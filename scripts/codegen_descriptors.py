"""
Parses C structs in src/vulkan/object/descriptor.h and generates wrappers
used by unified uniform buffer and shader descriptors.
"""
from utils import *

supported_field_type = ['float', 'uint', 'vec2', 'vec3', 'vec4', 'mat4']
# std430 alignments taken from https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_scalar_block_layout.txt
# alignment_name = "std430"
# uniform_buffer_alignments = {'float': 4, 'uint': 4, 'vec2': 8, 'vec3': 16, 'vec4': 16, 'mat4': 16}
# scalar alignments taken from https://github.com/KhronosGroup/GLSL/blob/master/extensions/ext/GL_EXT_scalar_block_layout.txt
alignment_name = "scalar"
uniform_buffer_alignments = {'float': 4, 'uint': 4, 'vec2': 4, 'vec3': 4, 'vec4': 4, 'mat4': 4}


def get_std430_alignment_for_basic_type(field_type):
    alignment = uniform_buffer_alignments.get(field_type, None)
    assert (alignment is not None)
    return alignment


def get_std430_for_helper_struct(struct_fields, field_info, helper_structs):
    alignment = max([get_std430_alignment(x[1], field_info, helper_structs) for x in struct_fields])
    return alignment


def get_std430_for_array(field_type, field_info, helper_structs):
    alignment = get_std430_alignment(field_type, field_info, helper_structs)
    return alignment


def get_std430_alignment(field_type, field_info, helper_structs):
    """Vulkan spec: 15.5.4. Offset and Stride Assignment"""
    if 'array' in field_info:
        return get_std430_for_array(field_type, {}, helper_structs)
    elif field_type.endswith("_helper_element"):
        helper_struct_name = field_type.partition("_helper_element")[0]
        return get_std430_for_helper_struct(helper_structs[helper_struct_name], field_info, helper_structs)
    else:
        return get_std430_alignment_for_basic_type(field_type)


def get_field_array_str(field_info):
    if 'array' in field_info:
        return f'[{field_info["array"]}]'
    else:
        return ''


def parse_struct_fields(struct_decl):
    struct_fields = []
    for struct_field in struct_decl.get_children():
        field_name = struct_field.spelling
        field_type = struct_field.type.spelling.replace("_helper_struct", "_helper_element")
        field_info = {}
        if struct_field.brief_comment:
            for info in struct_field.brief_comment.split(','):
                key, _, value = [x.strip() for x in info.partition('=')]
                field_info[key] = value
        struct_fields.append((field_name, field_type, field_info))
    return struct_fields


def get_descriptor_h_str():
    descriptor_h_path = (src_path / "vulkan" / "objects" / "descriptor.h")
    s = []
    for supported_type in supported_field_type:
        s.append(f'typedef struct {supported_type} {supported_type};')
    s.append(descriptor_h_path.read_text())
    return '\n'.join(s)


def codegen_descriptors():
    push_constant_structs = {}
    uniform_buffer_structs = {}
    helper_structs = {}
    decls = ['#pragma once', '#include "../vulkan/constants.h"', '#include "../core/junk.h"', '']
    defs = ['#include "descriptors.h"', '#include "../core/core.h"', '']

    # parse C structs ending with _struct
    struct_decls = find_c_decls_in_str(get_descriptor_h_str(), clang.cindex.CursorKind.STRUCT_DECL)
    for struct_decl in struct_decls:
        if struct_decl.spelling.endswith('_push_constant_struct'):
            struct_name = struct_decl.spelling.partition('_push_constant_struct')[0]
            push_constant_structs[struct_name] = parse_struct_fields(struct_decl)
        if struct_decl.spelling.endswith('_uniform_buffer_struct'):
            struct_name = struct_decl.spelling.partition('_uniform_buffer_struct')[0]
            uniform_buffer_structs[struct_name] = parse_struct_fields(struct_decl)
        if struct_decl.spelling.endswith('_helper_struct'):
            struct_name = struct_decl.spelling.partition('_helper_struct')[0]
            helper_structs[struct_name] = parse_struct_fields(struct_decl)

    print(f'uniform_buffer_structs: {uniform_buffer_structs}')
    print(f'push_constant_structs: {push_constant_structs}')
    print(f'helper_structs: {helper_structs}')

    # generate aligned _helper_element
    for struct_name, struct_fields in helper_structs.items():
        struct_name = struct_name + '_helper_element'
        decls.append(f'typedef struct PACKED_STRUCT {struct_name} {{')
        for field_name, field_type, field_info in struct_fields:
            alignment = get_std430_alignment(field_type, field_info, helper_structs)
            decls.append(f'  alignas({alignment}) {field_type} {field_name} {get_field_array_str(field_info)};')
        decls.append(f'}} {struct_name};\n')

    # generate aligned _push_constant_element
    for struct_name, struct_fields in push_constant_structs.items():
        struct_name = struct_name + '_push_constant_element'
        decls.append(f'typedef struct PACKED_STRUCT {struct_name} {{')
        for field_name, field_type, field_info in struct_fields:
            alignment = get_std430_alignment(field_type, field_info, helper_structs)
            decls.append(f'  alignas({alignment}) {field_type} {field_name} {get_field_array_str(field_info)};')
        decls.append(f'}} {struct_name};\n')

    # generate aligned _uniform_buffer_element
    for struct_name, struct_fields in uniform_buffer_structs.items():
        struct_name = struct_name + '_uniform_buffer_element'
        decls.append(f'typedef struct PACKED_STRUCT {struct_name} {{')
        for field_name, field_type, field_info in struct_fields:
            alignment = get_std430_alignment(field_type, field_info, helper_structs)
            decls.append(f'  alignas({alignment}) {field_type} {field_name} {get_field_array_str(field_info)};')
        decls.append(f'}} {struct_name};\n')

    # generate GLSL strings for push constants
    for struct_name, struct_fields in push_constant_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_push_constant(UT_string *s);')
        defs.append(
            f'void glsl_add_{struct_name}_push_constant(UT_string *s) {{')
        instance_name = struct_name
        block_name = instance_name + "Block"

        defs.append(f'  utstring_printf(s, "layout(push_constant) uniform {block_name} {{\\n");')
        for field_name, field_type, field_info in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name} {get_field_array_str(field_info)};\\n");')
        defs.append(f'  utstring_printf(s, "}} {instance_name};\\n");')
        defs.append(f'}}')

    # generate GLSL strings for uniform buffers
    for struct_name, struct_fields in uniform_buffer_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count);')
        defs.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {{')
        instance_name = struct_name
        block_name = instance_name + "Block"
        struct_name = instance_name + "Struct"

        defs.append(f'  utstring_printf(s, "struct {struct_name} {{\\n");')
        for field_name, field_type, field_info in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name} {get_field_array_str(field_info)};\\n");')
        defs.append(f'  utstring_printf(s, "}};\\n");')

        defs.append(
            f'  utstring_printf(s, "layout({alignment_name}, set = %u, binding = %u) uniform {block_name} {{\\n", set, binding);')
        defs.append(f'  utstring_printf(s, "  {struct_name} {instance_name}");')
        defs.append(f'  if (count > 1) {{utstring_printf(s, "[%u]", count);}}')
        defs.append(f'  utstring_printf(s, ";\\n}};\\n");')
        defs.append(f'}}')

    # generate GLSL strings for helper structs
    for struct_name, struct_fields in helper_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_helper_element(UT_string *s);')
        defs.append(
            f'void glsl_add_{struct_name}_helper_element(UT_string *s) {{')

        defs.append(f'  utstring_printf(s, "struct {struct_name}_helper_element {{\\n");')
        for field_name, field_type, field_info in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name} {get_field_array_str(field_info)};\\n");')
        defs.append(f'  utstring_printf(s, "}};\\n");')
        defs.append(f'}}')

    # generate X-macro with push constant names
    decls.append(f'\n#define END_OF_VULKAN_PUSH_CONSTANTS')
    decls.append(f'#define VULKAN_PUSH_CONSTANTS(X, ...) \\')
    for struct_name in push_constant_structs.keys():
        struct_name = struct_name
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_PUSH_CONSTANTS')

    # generate X-macro with uniform buffer names
    decls.append(f'\n#define END_OF_VULKAN_UNIFORM_BUFFERS')
    decls.append(f'#define VULKAN_UNIFORM_BUFFERS(X, ...) \\')
    for struct_name in uniform_buffer_structs.keys():
        struct_name = struct_name
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_UNIFORM_BUFFERS')

    # generate X-macro with helper struct names
    decls.append(f'\n#define END_OF_VULKAN_HELPER_STRUCTS')
    decls.append(f'#define VULKAN_HELPER_STRUCTS(X, ...) \\')
    for struct_name in helper_structs.keys():
        struct_name = struct_name
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_HELPER_STRUCTS')

    output_strings(decls, get_output_path('descriptors.h'))
    output_strings(defs, get_output_path('descriptors.c'))

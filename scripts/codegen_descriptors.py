"""
Parses C structs in src/vulkan/object/descriptor.h and generates wrappers
used by unified uniform buffer and shader descriptors.
"""
from utils import *

supported_field_type = ['float', 'uint', 'vec2', 'vec3', 'vec4', 'mat4']
# alignments taken from https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
uniform_buffer_std140_alignments = {'float': 4, 'uint': 4, 'vec2': 8, 'vec3': 16, 'vec4': 16, 'mat4': 16}


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
    decls = ['#pragma once', '#include "../vulkan/constants.h"', '']
    defs = ['#include "descriptors.h"', '#include "../core/core.h"', '']

    # parse C structs ending with _struct
    struct_decls = find_c_decls_in_str(get_descriptor_h_str(), clang.cindex.CursorKind.STRUCT_DECL)
    for struct_decl in struct_decls:
        if struct_decl.spelling.endswith('_push_constant_struct'):
            struct_name = struct_decl.spelling.partition('_push_constant_struct')[0]
            struct_fields = []
            for struct_field in struct_decl.get_children():
                field_name = struct_field.spelling
                field_type = struct_field.type.spelling
                struct_fields.append((field_name, field_type))
            push_constant_structs[struct_name] = struct_fields
        if struct_decl.spelling.endswith('_uniform_buffer_struct'):
            struct_name = struct_decl.spelling.partition('_uniform_buffer_struct')[0]
            struct_fields = []
            for struct_field in struct_decl.get_children():
                field_name = struct_field.spelling
                field_type = struct_field.type.spelling
                field_comment = ''
                if struct_field.brief_comment:
                    field_comment = struct_field.brief_comment
                struct_fields.append((field_name, field_type, field_comment))
            uniform_buffer_structs[struct_name] = struct_fields
        if struct_decl.spelling.endswith('_helper_struct'):
            struct_name = struct_decl.spelling.partition('_helper_struct')[0]
            struct_fields = []
            for struct_field in struct_decl.get_children():
                field_name = struct_field.spelling
                field_type = struct_field.type.spelling
                struct_fields.append((field_name, field_type))
            helper_structs[struct_name] = struct_fields
    print(f'uniform_buffer_structs: {uniform_buffer_structs}')
    print(f'push_constant_structs: {push_constant_structs}')
    print(f'helper_structs: {helper_structs}')

    # generate aligned _helper_element
    for struct_name, struct_fields in helper_structs.items():
        struct_name = struct_name + '_helper_element'
        decls.append(f'typedef struct {struct_name} {{')
        for field_name, field_type in struct_fields:
            alignment = uniform_buffer_std140_alignments.get(field_type, 'unknown_alignment')
            decls.append(f'  alignas({alignment}) {field_type} {field_name};')
        decls.append(f'}} {struct_name};\n')

    # generate aligned _push_constant_element
    for struct_name, struct_fields in push_constant_structs.items():
        struct_name = struct_name + '_push_constant_element'
        decls.append(f'typedef struct {struct_name} {{')
        for field_name, field_type in struct_fields:
            alignment = uniform_buffer_std140_alignments.get(field_type, 'unknown_alignment')
            decls.append(f'  alignas({alignment}) {field_type} {field_name};')
        decls.append(f'}} {struct_name};\n')

    # generate aligned _uniform_buffer_element
    for struct_name, struct_fields in uniform_buffer_structs.items():
        struct_name = struct_name + '_uniform_buffer_element'
        decls.append(f'typedef struct {struct_name} {{')
        for field_name, field_type, field_comment in struct_fields:
            if field_type.endswith("_struct"):
                decls.append(f'  {field_type.replace("_struct", "_element")} {field_name} {field_comment};')
            else:
                alignment = uniform_buffer_std140_alignments.get(field_type, 'unknown_alignment')
                decls.append(f'  alignas({alignment}) {field_type} {field_name};')
        decls.append(f'}} {struct_name};\n')

    # generate GLSL strings for push constants
    for struct_name, struct_fields in push_constant_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_push_constant(UT_string *s);')
        defs.append(
            f'void glsl_add_{struct_name}_push_constant(UT_string *s) {{')
        instance_name = struct_name.partition("vulkan_")[2]
        block_name = instance_name + "Block"

        defs.append(f'  utstring_printf(s, "layout(push_constant) uniform {block_name} {{\\n");')
        for field_name, field_type in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name};\\n");')
        defs.append(f'  utstring_printf(s, "}} {instance_name};\\n");')
        defs.append(f'}}')

    # generate GLSL strings for uniform buffers
    for struct_name, struct_fields in uniform_buffer_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count);')
        defs.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {{')
        instance_name = struct_name.partition("vulkan_")[2]
        block_name = instance_name + "Block"
        struct_name = instance_name + "Struct"

        defs.append(f'  utstring_printf(s, "struct {struct_name} {{\\n");')
        for field_name, field_type, field_comment in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name} {field_comment};\\n");')
        defs.append(f'  utstring_printf(s, "}};\\n");')

        defs.append(f'  utstring_printf(s, "layout(set = %u, binding = %u) uniform {block_name} {{\\n", set, binding);')
        defs.append(f'  utstring_printf(s, "  {struct_name} {instance_name}");')
        defs.append(f'  if (count > 1) {{utstring_printf(s, "[%u]", count);}}')
        defs.append(f'  utstring_printf(s, ";\\n}};\\n");')
        defs.append(f'}}')

    # generate GLSL strings for helper structs
    for struct_name, struct_fields in helper_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_helper_struct(UT_string *s);')
        defs.append(
            f'void glsl_add_{struct_name}_helper_struct(UT_string *s) {{')

        defs.append(f'  utstring_printf(s, "struct {struct_name}_helper_struct {{\\n");')
        for field_name, field_type in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name};\\n");')
        defs.append(f'  utstring_printf(s, "}};\\n");')
        defs.append(f'}}')

    # generate X-macro with push constant names
    decls.append(f'\n#define END_OF_VULKAN_PUSH_CONSTANTS')
    decls.append(f'#define VULKAN_PUSH_CONSTANTS(X, ...) \\')
    for struct_name in push_constant_structs.keys():
        struct_name = struct_name.partition("vulkan_")[2]
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_PUSH_CONSTANTS')

    # generate X-macro with uniform buffer names
    decls.append(f'\n#define END_OF_VULKAN_UNIFORM_BUFFERS')
    decls.append(f'#define VULKAN_UNIFORM_BUFFERS(X, ...) \\')
    for struct_name in uniform_buffer_structs.keys():
        struct_name = struct_name.partition("vulkan_")[2]
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_UNIFORM_BUFFERS')

    # generate X-macro with helper struct names
    decls.append(f'\n#define END_OF_VULKAN_HELPER_STRUCTS')
    decls.append(f'#define VULKAN_HELPER_STRUCTS(X, ...) \\')
    for struct_name in helper_structs.keys():
        struct_name = struct_name.partition("vulkan_")[2]
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_HELPER_STRUCTS')

    output_strings(decls, get_output_path('descriptors.h'))
    output_strings(defs, get_output_path('descriptors.c'))

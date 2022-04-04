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
    uniform_structs = {}
    decls = ['#pragma once', '']
    defs = ['#include "descriptors.h"', '#include "../core/core.h"', '']

    # parse C structs ending with _struct
    struct_decls = find_c_decls_in_str(get_descriptor_h_str(), clang.cindex.CursorKind.STRUCT_DECL)
    for struct_decl in struct_decls:
        if struct_decl.spelling.endswith('_uniform_struct'):
            struct_name = struct_decl.spelling.partition('_uniform_struct')[0]
            struct_fields = []
            for struct_field in struct_decl.get_children():
                field_name = struct_field.spelling
                field_type = struct_field.type.spelling
                struct_fields.append((field_name, field_type))
            uniform_structs[struct_name] = struct_fields
    print(f'uniform_structs: {uniform_structs}')

    # generate aligned _uniform_element
    for struct_name, struct_fields in uniform_structs.items():
        struct_name = struct_name + '_uniform_buffer_element'
        decls.append(f'typedef struct {struct_name} {{')
        for field_name, field_type in struct_fields:
            alignment = uniform_buffer_std140_alignments.get(field_type, 'unknown_alignment')
            decls.append(f'  alignas({alignment}) {field_type} {field_name};')
        decls.append(f'}} {struct_name};\n')

    # generate GLSL strings
    for struct_name, struct_fields in uniform_structs.items():
        decls.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count);')
        defs.append(
            f'void glsl_add_{struct_name}_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {{')
        instance_name = struct_name.partition("vulkan_")[2]
        block_name = instance_name + "Block"
        struct_name = instance_name + "Struct"

        defs.append(f'  utstring_printf(s, "struct {struct_name} {{\\n");')
        for field_name, field_type in struct_fields:
            defs.append(f' utstring_printf(s, "  {field_type} {field_name};\\n");')
        defs.append(f'  utstring_printf(s, "}};\\n");')

        defs.append(f'  utstring_printf(s, "layout(set = %u, binding = %u) uniform {block_name} {{\\n", set, binding);')
        defs.append(f'  utstring_printf(s, "  {struct_name} {instance_name}");')
        defs.append(f'  if (count > 1) {{utstring_printf(s, "[%u]", count);}}')
        defs.append(f'  utstring_printf(s, ";\\n}};\\n");')
        defs.append(f'}}')

    # generate X-macro with uniform buffer names
    decls.append(f'\n#define END_OF_VULKAN_UNIFORM_BUFFERS')
    decls.append(f'#define VULKAN_UNIFORM_BUFFERS(X, ...) \\')
    for struct_name in uniform_structs.keys():
        struct_name = struct_name.partition("vulkan_")[2]
        decls.append(f'  X({struct_name}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_VULKAN_UNIFORM_BUFFERS')

    output_strings(decls, get_output_path('descriptors.h'))
    output_strings(defs, get_output_path('descriptors.c'))

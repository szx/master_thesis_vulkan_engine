"""
Parses C structs in src/vulkan/object/descriptor.h and generates wrappers
used by unified uniform buffer and shader descriptors.
"""
from utils import *

supported_field_type = ['float', 'vec2', 'vec3', 'vec4', 'mat4']
# alignments taken from https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_uniform_buffer_object.txt
uniform_buffer_std140_alignments = {'float': 4, 'vec2': 8, 'vec3': 16, 'vec4': 16, 'mat4': 16}


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
    defs = ['#include "macros.h"', '#include "../core/core.h"', '']

    # parse C structs ending with _struct
    struct_decls = find_c_decls_in_str(get_descriptor_h_str(), clang.cindex.CursorKind.STRUCT_DECL)
    for struct_decl in struct_decls:
        struct_name = struct_decl.spelling
        struct_fields = []
        for struct_field in struct_decl.get_children():
            field_name = struct_field.spelling
            field_type = struct_field.type.spelling
            struct_fields.append((field_name, field_type))
        if struct_name.endswith('_uniform_struct'):
            uniform_structs[struct_name] = struct_fields
    print(f'uniform_structs: {uniform_structs}')

    # generate aligned _uniform_element
    for struct_name, struct_fields in uniform_structs.items():
        struct_name = struct_name.partition('_struct')[0] + '_element'
        decls.append(f'typedef struct {struct_name} {{')
        for field_name, field_type in struct_fields:
            alignment = uniform_buffer_std140_alignments.get(field_type, 'unknown_alignment')
            decls.append(f'  alignas({alignment}) {field_type} {field_name};')
        decls.append(f'}} {struct_name};\n')

    # generate _uniform_buffer
    # HIRO _create, _destroy, allocate buffer, allow for array of elements.

    output_strings(decls, get_output_path('descriptors.h'))
    output_strings(defs, get_output_path('descriptors.c'))

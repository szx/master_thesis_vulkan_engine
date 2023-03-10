from utils import *
from dataclasses import dataclass


@dataclass
class EnumDeclaration:
    """Class for keeping track of an item in inventory."""
    name: str
    enumerators: ...

    def is_flags(self) -> bool:
        found_flag_num = 0
        skip_values = [0, 1, 2]
        max_vulkan_enum_value = 0x7FFFFFFF
        for (name, value) in self.enumerators:
            if value == max_vulkan_enum_value:
                continue
            if value in skip_values:
                continue
            if value & (value - 1) != 0:
                return False
            else:
                found_flag_num += 1
        return found_flag_num > 1


def find_enum_decls(paths):
    """
    Returns unique enum declarations.
    """
    unique_enum_decls = []

    def walk(node):
        # print(f"node: {node.displayname} {node.spelling} {node.kind} {node.location}")
        if node.kind == clang.cindex.CursorKind.ENUM_DECL and node.spelling != '':
            if not any([x.name == node.spelling for x in unique_enum_decls]):
                # print(f"found enum: {node.spelling} {node.kind}")
                enum = EnumDeclaration(node.spelling, [])
                for childNode in node.get_children():
                    assert (childNode.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL)
                    enumerator_name = str(childNode.spelling)
                    enumerator_value = int(childNode.enum_value)
                    # print(f"found child: {enumerator_name} {enumerator_value}")
                    enum.enumerators.append((enumerator_name, enumerator_value))
                unique_enum_decls.append(enum)
        for c in node.get_children():
            walk(c)

    index = clang.cindex.Index.create()
    for path in paths:
        walk(index.parse(path, args=[f'-I{get_vulkan_sdk_path()}/include']).cursor)

    unique_enum_decls.sort(key=lambda x: x.name)
    return unique_enum_decls


def enum_helpers_decls_template(e):
    if not e.is_flags():
        return f"const char *{e.name}_debug_str(int value);"
    else:
        return f"const char *{e.name}_debug_str(int value);\nvoid {e.name}_debug_print(int flags, int indent);"


def enum_helpers_defs_template(e):
    s = []
    s.append(f'const char *{e.name}_debug_str(int value) {{')
    for name, value in e.enumerators:
        s.append(f'  if (value == {value}) {{ return "{name}"; }}')
    s.append(f'  return "UNKNOWN {e.name}";')
    s.append(f'}}')
    if e.is_flags():
        s.append(f'void {e.name}_debug_print(int flags, int indent) {{')
        s.append(f'  log_debug(INDENT_FORMAT_STRING "{e.name}: ", INDENT_FORMAT_ARGS(0));')
        for name, value in e.enumerators:
            s.append(
                f'  if ((flags & {value}) != 0) {{ log_debug(INDENT_FORMAT_STRING "{name}", INDENT_FORMAT_ARGS(2)); }}')
        s.append(f'}}')
    return '\n'.join(s);


def codegen_meta():
    header_paths = get_header_paths()
    enum_decls = find_enum_decls(header_paths)

    decls = []
    defs = ['#include "../core/core.h"']
    decls.extend(generate_strings(enum_decls, enum_helpers_decls_template))
    defs.extend(generate_strings(enum_decls, enum_helpers_defs_template))

    output_strings(decls, get_output_path('meta.h'))
    output_strings(defs, get_output_path('meta.c'))

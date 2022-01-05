#!/usr/bin/env python3
import os
import clang.cindex
from dataclasses import dataclass

script_path = os.path.realpath(__file__)
root_path = os.path.realpath(os.path.join(os.path.dirname(script_path), '..'))
src_path = os.path.join(root_path, 'src')


def get_header_paths():
    for dirname, dirnames, filenames in os.walk(src_path):
        if os.path.basename(dirname) == 'codegen':
            continue
        for filename in filenames:
            if filename.endswith('.h'):
                yield os.path.join(dirname, filename)


print(get_header_paths())


# HIRO: Introspection.
@dataclass
class EnumDeclaration:
    """Class for keeping track of an item in inventory."""
    name: str
    enumerators: ...

    def is_flags(self) -> bool:
        found_flag_num = 0
        none_value = 0x0
        max_vulkan_enum_value = 0x7FFFFFFF
        for (name, value) in self.enumerators:
            if value == max_vulkan_enum_value:
                continue
            if value == none_value:
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
        # print(f"node: {node.displayname} {node.spelling} {node.kind}")
        if node.kind == clang.cindex.CursorKind.ENUM_DECL and node.spelling != '':
            if not any([x.name == node.spelling for x in unique_enum_decls]):
                print(f"found enum: {node.spelling} {node.kind}")
                enum = EnumDeclaration(node.spelling, [])
                for childNode in node.get_children():
                    assert (childNode.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL)
                    enumerator_name = str(childNode.spelling)
                    enumerator_value = int(childNode.enum_value)
                    print(f"found child: {enumerator_name} {enumerator_value}")
                    enum.enumerators.append((enumerator_name, enumerator_value))
                unique_enum_decls.append(enum)
        for c in node.get_children():
            walk(c)

    index = clang.cindex.Index.create()
    for path in paths:
        print(f"parsing enums decls in {path}")
        walk(index.parse(path).cursor)

    unique_enum_decls.sort(key=lambda x: x.name)
    return unique_enum_decls


header_paths = get_header_paths()
enum_decls = find_enum_decls(header_paths)


def generate_decls_and_defs(decls, defs, elements, decls_template, defs_template):
    for e in elements:
        decls.append(decls_template(e))
        defs.append(defs_template(e))


def output_decls_and_defs(decls, defs, decls_path, defs_path):
    with open(decls_path, 'w') as f:
        f.write('\n'.join(decls))
        f.write('\n')
    with open(defs_path, 'w') as f:
        f.write('\n'.join(defs))
        f.write('\n')


def enum_helpers_decls_template(e):
    if not e.is_flags():
        return f"const char *{e.name}_debug_str(int value);"
    else:
        return f"void {e.name}_debug_print(int flags);"


def enum_helpers_defs_template(e):
    s = []
    if not e.is_flags():
        s.append(f'const char *{e.name}_debug_str(int value) {{')
        for name, value in e.enumerators:
            s.append(f'  if (value == {value}) {{ return "{name}"; }}')
        s.append(f'  return "UNKNOWN {e.name}";')
    else:
        s.append(f'void {e.name}_debug_print(int flags) {{')
        s.append(f'  log_debug("{e.name}: {{");')
        for name, value in e.enumerators:
            s.append(f'  if ((flags & {value}) != 0) {{ log_debug("  {name}"); }}')
        s.append(f'  log_debug("{e.name}: }}");')
    s.append(f'}}')
    return '\n'.join(s);


decls = []
defs = []
generate_decls_and_defs(decls, defs, enum_decls,
                        enum_helpers_decls_template,
                        enum_helpers_defs_template)

codegen_meta_decls_path = os.path.join(src_path, 'codegen', 'meta.h')
codegen_meta_defs_path = os.path.join(src_path, 'codegen', 'meta.c')
output_decls_and_defs(decls, defs, codegen_meta_decls_path, codegen_meta_defs_path)

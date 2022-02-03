from utils import *
from collections import defaultdict


def codegen_headers():
    header_paths = get_header_paths()
    src = defaultdict(list)
    for header in header_paths:
        rel_dirname = os.path.dirname(get_relative_path(src_path, header))
        dirname = os.path.basename(rel_dirname) if "/" not in rel_dirname else os.path.dirname(rel_dirname)
        filename = get_relative_path(src_path / dirname, header)
        if dirname == filename.strip('.h'):
            continue
        src[dirname].append(filename)

    for dirname, headers in src.items():
        header_path = os.path.join(src_path, dirname, f'{dirname}.h')
        includes = [f'#include "{x}"' for x in headers]
        print(header_path, includes)
        output_strings(includes, header_path)

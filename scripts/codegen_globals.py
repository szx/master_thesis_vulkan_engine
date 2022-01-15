from utils import *

global_paths = [
    ("assetsFilePath", ("assets", "data.db")),
    ("configFilePath", ("assets", "config.ini"))
]


def global_decls_template(e):
    if isinstance(e, tuple):
        return f"  UT_string *{e[0]};"


def global_defs_template(e):
    if isinstance(e, tuple):
        return f'UT_string *{e[0]} = NULL;'


def global_create_template(e):
    init = []
    if isinstance(e, tuple):
        init.append(f'  globals.{e[0]} = get_executable_dir_file_path("{e[1][0]}", "{e[1][1]}");')
    return '\n'.join(init)


def global_destroy_template(e):
    init = []
    if isinstance(e, tuple):
        init.append(f'  utstring_free(globals.{e[0]});')
    return '\n'.join(init)


def codegen_globals():
    decls = ['#pragma once', 'void globals_create();', 'void globals_destroy();']
    defs = ['#include "globals.h"', '#include "../core/platform.h"']

    decls.append('typedef struct globals_ {')
    decls.extend(generate_strings(global_paths, global_decls_template))
    decls.append('} globals_;')
    decls.append('extern globals_ globals;')

    defs.append('globals_ globals;')
    defs.append('void globals_create() {')
    defs.extend(generate_strings(global_paths, global_create_template))
    defs.append('}')
    defs.append('void globals_destroy() {')
    defs.extend(generate_strings(global_paths, global_destroy_template))
    defs.append('}')

    output_strings(decls, get_output_path('globals.h'))
    output_strings(defs, get_output_path('globals.c'))

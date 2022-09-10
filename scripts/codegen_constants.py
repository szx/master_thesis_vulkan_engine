from utils import *


def constant_decls_template(e):
    if isinstance(e, tuple):
        return f"#define {e[0]} {e[1]}"


def codegen_constants(config: ConfigParser):
    constant_config = [(key, config['CONSTANTS'][key]) for key in config['CONSTANTS']]

    decls = ['#pragma once']
    defs = ['#include "constants.h"', '#include "../core/platform.h"']

    decls.extend(generate_strings(constant_config, constant_decls_template))
    decls.append(f'\n#define END_OF_CODEGEN_CONSTANTS')
    decls.append(f'#define CODEGEN_CONSTANTS(X, ...) \\')
    for key, value in constant_config:
        decls.append(f'  X({key}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_CODEGEN_CONSTANTS')

    output_strings(decls, get_output_path('constants.h'))
    output_strings(defs, get_output_path('constants.c'))

"""
Generates X-macros used by INI config parser and SQLite asset database.
"""
from utils import *


def get_ini_config_by_section(config, name):
    def parse_value(v):
        v = v.strip('"')
        try:
            v = int(v)
        except ValueError:
            pass
        return v

    ini_config = [(key.split('.'), config[name][key]) for key in config[name]]
    sections = set([e[0][0] for e in ini_config])
    ini_config = {section: {k[1]: parse_value(v) for (k, v) in ini_config if k[0] == section} for section in sections}
    return ini_config


def macros_items_template(e):
    init = []
    section, items = e
    for (key, value) in items.items():
        if isinstance(value, int):
            init.append(f'int {key} {value}')
        elif isinstance(value, str):
            init.append(f'str {key} "{value}"')
    return '\n'.join(init)


def codegen_macros(config: ConfigParser):
    config_ini = get_ini_config_by_section(config, 'ASSET.CONFIG')

    decls = ['#pragma once']
    defs = ['#include "macros.h"', '#include "../core/platform.h"']

    decls.append(f'\n#define END_OF_DATA_CONFIG_SECTION')
    decls.append(f'#define DATA_CONFIG_SECTIONS(X, ...) \\')
    for section in config_ini.keys():
        decls.append(f'  X({section}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_DATA_CONFIG_SECTION')

    decls.append(f'\n#define END_OF_DATA_CONFIG_INT_KEYS')
    decls.append(f'#define DATA_CONFIG_INT_KEYS(X, ...) \\')
    for section, items in config_ini.items():
        for key, value in items.items():
            if isinstance(value, int):
                decls.append(f'  X({section}, {key}, {value}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_DATA_CONFIG_INT_KEYS')

    decls.append(f'\n#define END_OF_DATA_CONFIG_STR_KEYS')
    decls.append(f'#define DATA_CONFIG_STR_KEYS(X, ...) \\')
    for section, items in config_ini.items():
        for key, value in items.items():
            if isinstance(value, str):
                decls.append(f'  X({section}, {key}, "{value}", __VA_ARGS__) \\')
    decls.append(f'  END_OF_DATA_CONFIG_STR_KEYS')

    # HIRO assets.h

    output_strings(decls, get_output_path('macros.h'))
    output_strings(defs, get_output_path('macros.c'))

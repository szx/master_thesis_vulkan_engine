"""
Generates X-macros used by INI config parser and SQLite asset database.
"""
from utils import *


def get_config(config, name, split_keys=False):
    def parse_value(v):
        v = v.strip('"')
        try:
            v = int(v)
        except ValueError:
            pass
        return v

    if split_keys:
        ini_config = [(key.split('.'), config[name][key]) for key in config[name]]
        sections = set([e[0][0] for e in ini_config])
        ini_config = {section: {k[1]: parse_value(v) for (k, v) in ini_config if k[0] == section} for section in
                      sections}
    else:
        ini_config = [(key, parse_value(config[name][key])) for key in config[name]]
        ini_config = dict(ini_config)
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
    decls = ['#pragma once']
    defs = ['#include "macros.h"', '#include "../core/platform.h"']

    # config.h
    config_ini = get_config(config, 'ASSET.CONFIG', split_keys=True)

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

    # asset_db.h
    asset_db_ini = get_config(config, 'ASSET.DB')
    decls.append(f'\n#define END_OF_DATA_ASSET_DB_TABLES')
    decls.append(f'#define DATA_ASSET_DB_TABLES(X, ...) \\')
    for key, value in asset_db_ini.items():
        key_def, _, value_defs = value.partition(",")
        decls.append(f'  X({key}, "{key_def.strip()}", "{value_defs.strip()}", __VA_ARGS__) \\')
    decls.append(f'  END_OF_DATA_ASSET_DB_TABLES')

    decls.append(f'\n#define END_OF_DATA_ASSET_DB_COLUMNS')
    decls.append(f'#define DATA_ASSET_DB_COLUMNS(X, ...) \\')
    for key, value in asset_db_ini.items():
        for column, _, sqliteType in [value_def.strip().partition(' ') for value_def in value.split(',')]:
            decls.append(f'  X({key}, {column}, {sqliteType.lower()}, __VA_ARGS__) \\')
    decls.append(f'  END_OF_DATA_ASSET_DB_COLUMNS')

    output_strings(decls, get_output_path('macros.h'))
    output_strings(defs, get_output_path('macros.c'))

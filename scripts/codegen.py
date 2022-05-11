#!/usr/bin/env python3
from utils import parse_config
from codegen_meta import codegen_meta
from codegen_headers import codegen_headers
from codegen_globals import codegen_globals
from codegen_macros import codegen_macros
from codegen_descriptors import codegen_descriptors


def main():
    config = parse_config()
    # codegen_meta()
    # codegen_headers()
    codegen_globals(config)
    codegen_macros(config)
    codegen_descriptors()


if __name__ == '__main__':
    main()

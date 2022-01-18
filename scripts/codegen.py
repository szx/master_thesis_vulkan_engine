#!/usr/bin/env python3
from utils import parse_config
from codegen_meta import codegen_meta
from codegen_headers import codegen_headers
from codegen_globals import codegen_globals


def main():
    config = parse_config()
    codegen_meta()
    codegen_headers()
    codegen_globals(config)


if __name__ == '__main__':
    main()

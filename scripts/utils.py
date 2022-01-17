import os
import sys
from pathlib import Path
import subprocess
import shlex

script_path = Path(os.path.realpath(__file__))
root_path = script_path.parents[1]
src_path = root_path / 'src'


def get_header_paths():
    for dirname, dirnames, filenames in os.walk(src_path):
        if os.path.basename(dirname) == 'codegen':
            continue
        for filename in filenames:
            if filename.endswith('.h'):
                yield os.path.join(dirname, filename)


def get_output_path(filename):
    return os.path.join(src_path, 'codegen', filename)


def generate_strings(elements, template):
    for e in elements:
        yield template(e)


def output_strings(strings, path):
    with open(path, 'w') as f:
        f.write('/* This file was autogenerated. */\n\n')
        f.write('\n'.join(strings))
        f.write('\n')


def execute_command(command):
    process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE)
    output, error = process.communicate()
    return output, error
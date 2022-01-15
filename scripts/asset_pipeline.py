#!/usr/bin/env python3
from utils import *
from globals import *


def get_gltf_files(assets_path):
    gltf_names, gltf_paths = [], []
    for gltf_path in assets_path.glob('**/*.gltf'):
        gltf_name = gltf_path.stem
        assert (gltf_name not in gltf_names)  # Forbid two glTF files with same name.
        gltf_paths.append(gltf_path)
        gltf_names.append(gltf_name)
    return gltf_names, gltf_paths


def main(bin_path):
    assets_path = bin_path / assets_dirname
    gltf_names, gltf_paths = get_gltf_files(assets_path)
    print(gltf_names, gltf_paths)
    asset_pipeline_bin_path = bin_path / "asset_pipeline"
    print("running asset_pipeline executable")
    gltf_count = len(gltf_names)
    gltf_paths = [f'"{str(x)}"' for x in gltf_paths]
    args = f'{gltf_count} {" ".join(gltf_names)} {" ".join(gltf_paths)}'
    output, error = execute_command(f'{asset_pipeline_bin_path} {args}')
    if error:
        sys.exit(1)


if __name__ == '__main__':
    main(Path(sys.argv[1]))

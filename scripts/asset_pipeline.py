#!/usr/bin/env python3
from utils import *

def run_asset_pipeline(asset_pipeline_bin, source_asset_type, args):
    print(f"running asset_pipeline for '{source_asset_type}' '{args}'")
    output, error = execute_command(f'{asset_pipeline_bin} {source_asset_type} {args}')
    if error:
        print("error in asset pipeline:", output)
        sys.exit(1)


def get_gltf_files(assets_path):
    gltf_names, gltf_paths = [], []
    for gltf_path in assets_path.glob('**/*.gltf'):
        gltf_name = gltf_path.stem
        assert (gltf_name not in gltf_names)  # Forbid two glTF files with same name.
        gltf_paths.append(gltf_path)
        gltf_names.append(gltf_name)
    return gltf_names, gltf_paths


def main(bin_path):
    config = parse_config()
    assets_path = bin_path / config['GLOBALS']['assetsDirname']
    gltf_names, gltf_paths = get_gltf_files(assets_path)
    print(gltf_names, gltf_paths)
    asset_pipeline_bin_path = bin_path / "asset_pipeline"

    run_asset_pipeline(asset_pipeline_bin_path, "empty_config", '')
    run_asset_pipeline(asset_pipeline_bin_path, "empty_assets", '')

    gltf_paths = [f'"{str(x)}"' for x in gltf_paths]
    for gltf_name, gltf_path in zip(gltf_names, gltf_paths):
        args = f'{gltf_name} {gltf_path}'
        run_asset_pipeline(asset_pipeline_bin_path, "gltf", f'{gltf_name} {gltf_path}')
    # sys.exit(1)


if __name__ == '__main__':
    main(Path(sys.argv[1]))

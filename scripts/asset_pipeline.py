#!/usr/bin/env python3
from utils import *


def run_asset_pipeline(asset_pipeline_bin, source_asset_type, args):
    print(f"running asset_pipeline for '{source_asset_type}' '{args}'")
    output, error = execute_command(f'{asset_pipeline_bin} {source_asset_type} {args}')
    if error:
        print("error in asset pipeline:", output)
        sys.exit(1)


def get_cubemap_files(assets_path):
    cubemaps = []
    for path in assets_path.glob('cubemap/*/'):
        if not path.is_dir():
            continue
        name = path.stem
        ext = list(map(lambda x: x.suffix[1:], path.glob('*')))
        assert (len(ext) == 6 and ext.count(ext[0]) == len(ext))
        ext = ext[0]
        cubemaps.append((name, path, ext))
    return cubemaps


def get_gltf_files(assets_path):
    gltf_names, gltf_paths = [], []
    for gltf_path in assets_path.glob('**/*.gltf'):
        gltf_name = gltf_path.stem.lower()
        assert (gltf_name not in gltf_names)  # Forbid two glTF files with same name.
        gltf_paths.append(gltf_path)
        gltf_names.append(gltf_name)
    return gltf_names, gltf_paths


def main(bin_path):
    config = parse_config()
    assets_path = bin_path / config['GLOBALS']['assetsDirname']

    cubemaps = get_cubemap_files(assets_path)
    print(cubemaps)
    gltf_names, gltf_paths = get_gltf_files(assets_path)
    print(assets_path, gltf_names, gltf_paths)
    asset_pipeline_bin_path = bin_path / "asset_pipeline"

    run_asset_pipeline(asset_pipeline_bin_path, "empty_config", '')
    run_asset_pipeline(asset_pipeline_bin_path, "empty_assets", '')

    for name, path, ext in cubemaps:
        run_asset_pipeline(asset_pipeline_bin_path, "cubemap", f'{name} {path} {ext}')

    gltf_paths = [f'"{str(x)}"' for x in gltf_paths]
    for gltf_name, gltf_path in zip(gltf_names, gltf_paths):
        run_asset_pipeline(asset_pipeline_bin_path, "gltf", f'{gltf_name} {gltf_path}')


if __name__ == '__main__':
    main(Path(sys.argv[1]))

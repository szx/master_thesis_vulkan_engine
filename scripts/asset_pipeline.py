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


def get_font_files(assets_path):
    fonts = []
    for gltf_path in assets_path.glob('font/*.ttf'):
        name = gltf_path.stem
        path = gltf_path
        fonts.append((name, path))
    return fonts


def get_gltf_files(assets_path):
    gltfs = []
    for gltf_path in assets_path.glob('**/*.gltf'):
        name = gltf_path.stem
        path = gltf_path.parent
        gltfs.append((name, path))
    return gltfs


def main(bin_path):
    config = parse_config()
    assets_path = bin_path / config['GLOBALS']['assetsDirname']

    cubemaps = get_cubemap_files(assets_path)
    print(cubemaps)
    fonts = get_font_files(assets_path)
    print(fonts)
    gltfs = get_gltf_files(assets_path)
    print(assets_path, gltfs)
    asset_pipeline_bin_path = bin_path / "asset_pipeline"

    run_asset_pipeline(asset_pipeline_bin_path, "empty_config", '')
    run_asset_pipeline(asset_pipeline_bin_path, "empty_assets", '')

    for name, path, ext in cubemaps:
        run_asset_pipeline(asset_pipeline_bin_path, "cubemap", f'"{name}" "{path}" {ext}')

    for name, path in fonts:
        run_asset_pipeline(asset_pipeline_bin_path, "font", f'"{name}" "{path}"')

    for name, path in gltfs:
        run_asset_pipeline(asset_pipeline_bin_path, "gltf", f'"{name}" "{path}"')


if __name__ == '__main__':
    main(Path(sys.argv[1]))

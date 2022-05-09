#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

typedef struct asset_pipeline_input {
  UT_string *sourceAssetType;
  UT_string *sourceAssetName;
  UT_string *sourceAssetPath;
  UT_string *sourceAssetExt;
} asset_pipeline_input;

asset_pipeline_input *asset_pipeline_input_create() {
  asset_pipeline_input *inputs = core_alloc(sizeof(asset_pipeline_input));
  utstring_new(inputs->sourceAssetType);
  utstring_new(inputs->sourceAssetName);
  utstring_new(inputs->sourceAssetPath);
  utstring_new(inputs->sourceAssetExt);
  return inputs;
}

void asset_pipeline_input_destroy(asset_pipeline_input *input) {
  utstring_free(input->sourceAssetType);
  utstring_free(input->sourceAssetName);
  utstring_free(input->sourceAssetPath);
  utstring_free(input->sourceAssetExt);
  core_free(input);
}

void parse_arguments(int argc, char *argv[], asset_pipeline_input **assetInput) {
  verify(argc == 2 || argc == 4 || argc == 5);
  *assetInput = asset_pipeline_input_create();
  utstring_printf((*assetInput)->sourceAssetType, "%s", argv[1]);
  if (argc >= 3) {
    utstring_printf((*assetInput)->sourceAssetName, "%s", argv[2]);
  }
  if (argc >= 4) {
    utstring_printf((*assetInput)->sourceAssetPath, "%s", argv[3]);
  }
  if (argc >= 5) {
    utstring_printf((*assetInput)->sourceAssetExt, "%s", argv[4]);
  }
}

void write_default_asset_config() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_config_set_default_values(config);
  data_config_save(config);
  data_config_destroy(config);
}

void write_meshes_to_assets(data_asset_db *assetDb, asset_pipeline_input *assetInput) {
  log_info("processing gltf '%s' in '%s'", utstring_body(assetInput->sourceAssetName),
           utstring_body(assetInput->sourceAssetPath));

  UT_string *gltfPath;
  utstring_alloc(gltfPath, utstring_body(assetInput->sourceAssetPath));
  utstring_printf(gltfPath, "/%s.gltf", utstring_body(assetInput->sourceAssetName));
  UT_string *configPath;
  utstring_alloc(configPath, utstring_body(assetInput->sourceAssetPath));
  utstring_printf(configPath, "/%s.ini", utstring_body(assetInput->sourceAssetName));

  vulkan_scene_data *sceneData = vulkan_scene_data_create_with_gltf_file(
      assetInput->sourceAssetName, gltfPath, configPath, assetDb);
  vulkan_scene_data_serialize(sceneData, assetDb);
  vulkan_scene_data_destroy(sceneData);

  utstring_free(gltfPath);
  utstring_free(configPath);
}

void write_cubemap_to_assets(data_asset_db *assetDb, asset_pipeline_input *assetInput) {
  log_info("processing cubemap '%s' in '%s.%s'", utstring_body(assetInput->sourceAssetName),
           utstring_body(assetInput->sourceAssetPath), utstring_body(assetInput->sourceAssetExt));

  vulkan_asset_image cubemapImage;
  vulkan_asset_image_init(&cubemapImage, NULL);
  utarray_clear(cubemapImage.data);

  const char *faceNames[] = {"right", "left", "top", "bottom", "back", "front"};
  size_t faceCount = array_size(faceNames);
  for (size_t i = 0; i < faceCount; i++) {
    UT_string *imagePath;
    utstring_alloc(imagePath, utstring_body(assetInput->sourceAssetPath));
    append_to_path(imagePath, faceNames[i]);
    utstring_printf(imagePath, ".%s", utstring_body(assetInput->sourceAssetExt));
    log_debug("%s", utstring_body(imagePath));
    int w, h, c;
    stbi_uc *pixels = stbi_load(utstring_body(imagePath), &w, &h, &c, STBI_default);
    utstring_free(imagePath);
    if (pixels == NULL && stbi_failure_reason()) {
      panic("image loading error: %s", stbi_failure_reason());
    }
    cubemapImage.width = w;
    cubemapImage.height = h;
    cubemapImage.depth = 1;
    cubemapImage.channels = c;
    cubemapImage.faceCount = faceCount;
    cubemapImage.type = vulkan_image_type_cubemap;
    size_t faceSize =
        cubemapImage.width * cubemapImage.height * cubemapImage.depth * cubemapImage.channels;
    if (utarray_len(cubemapImage.data) == 0) {
      utarray_resize(cubemapImage.data, faceCount * faceSize);
    }
    core_memcpy(utarray_eltptr(cubemapImage.data, i * faceSize), pixels, faceSize);
    core_free(pixels);
  }
  vulkan_asset_image_debug_print(&cubemapImage, 0);
  vulkan_asset_image_serialize(&cubemapImage, assetDb);

  if (strncmp("skybox", utstring_body(assetInput->sourceAssetName), strlen("skybox")) == 0) {

    vulkan_asset_sampler cubemapSampler;
    vulkan_asset_sampler_init(&cubemapSampler, NULL);

    vulkan_asset_texture cubemapTexture;
    vulkan_asset_texture_init(&cubemapTexture, NULL);
    cubemapTexture.image = &cubemapImage;
    cubemapTexture.sampler = &cubemapSampler;

    vulkan_asset_skybox skybox;
    vulkan_asset_skybox_init(&skybox, NULL);
    utstring_printf(skybox.name, "%s", utstring_body(assetInput->sourceAssetName));
    skybox.cubemapTexture = &cubemapTexture;
    vulkan_asset_skybox_serialize(&skybox, assetDb);
    vulkan_asset_skybox_deinit(&skybox);
  }

  vulkan_asset_image_deinit(&cubemapImage);
}

int main(int argc, char *argv[]) {
  platform_create();
  log_info("parsing arguments");
  asset_pipeline_input *input = NULL;
  parse_arguments(argc, argv, &input);
  log_info("source asset type: '%s'", utstring_body(input->sourceAssetType));
  if (strcmp("empty_config", utstring_body(input->sourceAssetType)) == 0) {
    log_info("write default config");
    write_default_asset_config();
  } else if (strcmp("empty_assets", utstring_body(input->sourceAssetType)) == 0) {
    log_info("save empty asset database");
    data_asset_db *assetDb = data_asset_db_create();
    data_asset_db_save_empty(assetDb);
    data_asset_db_destroy(assetDb);
  } else if (strcmp("gltf", utstring_body(input->sourceAssetType)) == 0) {
    log_info("write meshes to asset database");
    data_asset_db *assetDb = data_asset_db_create();
    write_meshes_to_assets(assetDb, input);
    data_asset_db_destroy(assetDb);
  } else if (strcmp("cubemap", utstring_body(input->sourceAssetType)) == 0) {
    log_info("write meshes to asset database");
    data_asset_db *assetDb = data_asset_db_create();
    write_cubemap_to_assets(assetDb, input);
    data_asset_db_destroy(assetDb);
  }
  log_info("finished asset pipeline");
  asset_pipeline_input_destroy(input);
  platform_destroy();
  return 0;
}

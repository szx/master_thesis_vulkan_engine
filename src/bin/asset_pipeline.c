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
  UT_string *sceneConfigPath;
  utstring_alloc(sceneConfigPath, utstring_body(assetInput->sourceAssetPath));
  utstring_printf(sceneConfigPath, "/%s.ini", utstring_body(assetInput->sourceAssetName));

  data_config *assetConfig =
      data_config_create(globals.assetConfigFilepath, data_config_type_asset);

  vulkan_scene_data *sceneData = vulkan_scene_data_create_with_gltf_file(
      assetInput->sourceAssetName, gltfPath, sceneConfigPath, assetConfig, assetDb);
  vulkan_scene_data_serialize(sceneData, assetDb);
  vulkan_scene_data_destroy(sceneData);

  data_config_destroy(assetConfig);

  utstring_free(gltfPath);
  utstring_free(sceneConfigPath);
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

void write_font_to_assets(data_asset_db *assetDb, asset_pipeline_input *assetInput) {
  log_info("processing font '%s' in '%s'", utstring_body(assetInput->sourceAssetName),
           utstring_body(assetInput->sourceAssetPath));

  UT_string *ttfContents = read_text_file(assetInput->sourceAssetPath);
  stbtt_fontinfo info;
  if (!stbtt_InitFont(&info, (unsigned char *)utstring_body(ttfContents), 0)) {
    log_error("failed to init font %s", utstring_body(assetInput->sourceAssetPath));
  }

  // all visible ASCII characters
  char *characters =
      " !\"#$%&\\'()*+,-./"
      "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
  int boxWidth = 32;
  int boxHeight = 32;
  int bitmapWidth = 512;
  int bitmapHeight = 512;
  assert((size_t)bitmapWidth * bitmapHeight >= strlen(characters) * boxWidth);

  int ascent;
  stbtt_GetFontVMetrics(&info, &ascent, NULL, NULL);
  float scale = stbtt_ScaleForPixelHeight(&info, boxHeight);
  ascent = roundf(ascent * scale);

  UT_array *bitmap;
  utarray_alloc(bitmap, sizeof(uint8_t));
  utarray_resize(bitmap, bitmapWidth * bitmapHeight);

  /* create bitmap from TTF */
  uint8_t *boxTopLeft = utarray_front(bitmap);
  for (size_t i = 0; i < strlen(characters); ++i) {
    int characterWidth, characterHeight, yOff;
    uint8_t *pixels = stbtt_GetCodepointBitmap(&info, scale, scale, characters[i], &characterWidth,
                                               &characterHeight, NULL, &yOff);

    if (i % (bitmapWidth / boxWidth) == 0 && i != 0) {
      boxTopLeft = (uint8_t *)utarray_front(bitmap) +
                   (i / (bitmapWidth / boxWidth)) * boxHeight * bitmapWidth;
    }

    for (int v = 0; v < characterHeight; v++) {
      uint8_t *x0 = boxTopLeft;              // character touches top-left corner
      x0 += (ascent + yOff) * bitmapWidth;   // character moved down into baseline
      x0 += (boxWidth - characterWidth) / 2; // character moved right into the middle of the box
      core_memcpy(x0 + v * bitmapWidth, pixels + v * characterWidth, characterWidth);
    }
    stbtt_FreeBitmap(pixels, NULL);

    boxTopLeft += boxWidth;
  }
  utstring_free(ttfContents);

  // stbi_write_png("test.png", bitmapWidth, bitmapHeight, 1, utarray_front(bitmap), bitmapWidth);

  /* serialize font bitmap image */
  vulkan_asset_image fontImage;
  vulkan_asset_image_init(&fontImage, NULL);
  utarray_resize(fontImage.data, utarray_len(bitmap));
  core_memcpy(utarray_front(fontImage.data), utarray_front(bitmap), utarray_size(bitmap));
  utarray_free(bitmap);
  fontImage.width = bitmapWidth;
  fontImage.height = bitmapHeight;
  fontImage.depth = 1;
  fontImage.channels = 1;
  fontImage.faceCount = 1;
  fontImage.type = vulkan_image_type_font_bitmap;

  vulkan_asset_image_debug_print(&fontImage, 0);
  vulkan_asset_image_serialize(&fontImage, assetDb);

  /* serialize font */
  vulkan_asset_sampler fontSampler;
  vulkan_asset_sampler_init(&fontSampler, NULL);

  vulkan_asset_texture fontTexture;
  vulkan_asset_texture_init(&fontTexture, NULL);
  fontTexture.image = &fontImage;
  fontTexture.sampler = &fontSampler;

  vulkan_asset_font font;
  vulkan_asset_font_init(&font, NULL);
  utstring_printf(font.name, "%s", utstring_body(assetInput->sourceAssetName));
  font.fontTexture = &fontTexture;
  utstring_printf(font.characters, "%s", characters);
  vulkan_asset_font_serialize(&font, assetDb);
  vulkan_asset_font_deinit(&font);

  vulkan_asset_image_deinit(&fontImage);
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
  } else if (strcmp("font", utstring_body(input->sourceAssetType)) == 0) {
    log_info("write font to asset database");
    data_asset_db *assetDb = data_asset_db_create();
    write_font_to_assets(assetDb, input);
    data_asset_db_destroy(assetDb);
  }
  log_info("finished asset pipeline");
  asset_pipeline_input_destroy(input);
  platform_destroy();
  return 0;
}

#include "texture.h"
#include "../scene/data.h"

/* image */

void asset_image_init(asset_image *image, scene_data *sceneData) {
  // NOTE: Default 1x1 RGBA image.
  image->width = 1;
  image->height = 1;
  image->depth = 1;
  image->channels = 4;
  image->faceCount = 1;
  image->type = image_type_material_base_color;

  utarray_alloc(image->data, sizeof(uint8_t));
  utarray_resize(image->data, image->channels);
  for (size_t i = 0; i < utarray_len(image->data); i++) {
    *(uint8_t *)utarray_eltptr(image->data, i) = 255;
  }

  VULKAN_ASSET_FIELD_DEFS(image, image)
}

void asset_image_deinit(asset_image *image) { utarray_free(image->data); }

data_key asset_image_calculate_key(asset_image *image) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &image->width, sizeof(image->width));
  HASH_UPDATE(hashState, &image->height, sizeof(image->height));
  HASH_UPDATE(hashState, &image->depth, sizeof(image->depth));
  HASH_UPDATE(hashState, &image->channels, sizeof(image->channels));
  HASH_UPDATE(hashState, &image->faceCount, sizeof(image->faceCount));
  HASH_UPDATE(hashState, &image->type, sizeof(image->type));
  HASH_UPDATE(hashState, utarray_front(image->data), utarray_size(image->data))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_image_serialize(asset_image *image, asset_db *assetDb) {
  image->key = asset_image_calculate_key(image);
  asset_db_insert_image_width_int(assetDb, image->key, data_int_temp(image->width));
  asset_db_insert_image_height_int(assetDb, image->key, data_int_temp(image->height));
  asset_db_insert_image_depth_int(assetDb, image->key, data_int_temp(image->depth));
  asset_db_insert_image_channels_int(assetDb, image->key, data_int_temp(image->channels));
  asset_db_insert_image_faceCount_int(assetDb, image->key, data_int_temp(image->faceCount));
  asset_db_insert_image_type_int(assetDb, image->key, data_int_temp(image->type));
  asset_db_insert_image_data_byte_array(assetDb, image->key, data_byte_array_temp(image->data));
}

void asset_image_deserialize(asset_image *image, asset_db *assetDb, data_key key) {
  image->key = key;
  image->width = asset_db_select_image_width_int(assetDb, image->key).value;
  image->height = asset_db_select_image_height_int(assetDb, image->key).value;
  image->depth = asset_db_select_image_depth_int(assetDb, image->key).value;
  image->channels = asset_db_select_image_channels_int(assetDb, image->key).value;
  image->faceCount = asset_db_select_image_faceCount_int(assetDb, image->key).value;
  image->type = asset_db_select_image_type_int(assetDb, image->key).value;
  data_byte_array data = asset_db_select_image_data_byte_array(assetDb, image->key);
  utarray_resize(image->data, utarray_len(data.values));
  core_memcpy(image->data->d, data.values->d, utarray_size(image->data));
  data_byte_array_deinit(&data);
}

void asset_image_debug_print(asset_image *image, int indent) {
  log_debug(INDENT_FORMAT_STRING "image:\n", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), image->key);
  log_debug(INDENT_FORMAT_STRING "width=%zu", INDENT_FORMAT_ARGS(2), image->width);
  log_debug(INDENT_FORMAT_STRING "height=%zu", INDENT_FORMAT_ARGS(2), image->height);
  log_debug(INDENT_FORMAT_STRING "depth=%zu", INDENT_FORMAT_ARGS(2), image->depth);
  log_debug(INDENT_FORMAT_STRING "channels=%zu", INDENT_FORMAT_ARGS(2), image->channels);
  log_debug(INDENT_FORMAT_STRING "faceCount=%zu", INDENT_FORMAT_ARGS(2), image->faceCount);
  log_debug(INDENT_FORMAT_STRING "type=%s", INDENT_FORMAT_ARGS(2),
            image_type_debug_str(image->type));
  log_debug(INDENT_FORMAT_STRING "data=%zu bytes", INDENT_FORMAT_ARGS(2), utarray_len(image->data));
}

/* sampler */

void asset_sampler_init(asset_sampler *sampler, scene_data *sceneData) {
  sampler->magFilter = VK_FILTER_NEAREST;
  sampler->minFilter = VK_FILTER_NEAREST;
  sampler->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VULKAN_ASSET_FIELD_DEFS(sampler, sampler)
}

void asset_sampler_deinit(asset_sampler *sampler) {}

data_key asset_sampler_calculate_key(asset_sampler *sampler) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &sampler->magFilter, sizeof(sampler->magFilter));
  HASH_UPDATE(hashState, &sampler->minFilter, sizeof(sampler->minFilter));
  HASH_UPDATE(hashState, &sampler->addressModeU, sizeof(sampler->addressModeU));
  HASH_UPDATE(hashState, &sampler->addressModeV, sizeof(sampler->addressModeV));
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_sampler_serialize(asset_sampler *sampler, asset_db *assetDb) {
  sampler->key = asset_sampler_calculate_key(sampler);
  asset_db_insert_sampler_magFilter_int(assetDb, sampler->key, data_int_temp(sampler->magFilter));
  asset_db_insert_sampler_minFilter_int(assetDb, sampler->key, data_int_temp(sampler->minFilter));
  asset_db_insert_sampler_addressWrapU_int(assetDb, sampler->key,
                                           data_int_temp(sampler->addressModeU));
  asset_db_insert_sampler_addressWrapV_int(assetDb, sampler->key,
                                           data_int_temp(sampler->addressModeV));
}

void asset_sampler_deserialize(asset_sampler *sampler, asset_db *assetDb, data_key key) {
  sampler->key = key;
  sampler->magFilter = asset_db_select_sampler_magFilter_int(assetDb, sampler->key).value;
  sampler->minFilter = asset_db_select_sampler_minFilter_int(assetDb, sampler->key).value;
  sampler->addressModeU = asset_db_select_sampler_addressWrapU_int(assetDb, sampler->key).value;
  sampler->addressModeV = asset_db_select_sampler_addressWrapV_int(assetDb, sampler->key).value;
}

void asset_sampler_debug_print(asset_sampler *sampler, int indent) {
  log_debug(INDENT_FORMAT_STRING "sampler:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), sampler->key);
  log_debug(INDENT_FORMAT_STRING "magFilter=%d", INDENT_FORMAT_ARGS(2), sampler->magFilter);
  log_debug(INDENT_FORMAT_STRING "minFilter=%d", INDENT_FORMAT_ARGS(2), sampler->minFilter);
  log_debug(INDENT_FORMAT_STRING "addressWrapU=%d", INDENT_FORMAT_ARGS(2), sampler->addressModeU);
  log_debug(INDENT_FORMAT_STRING "addressWrapV=%d", INDENT_FORMAT_ARGS(2), sampler->addressModeV);
}

/* texture */

void asset_texture_init(asset_texture *texture, scene_data *sceneData) {
  texture->image = NULL;
  texture->sampler = NULL;

  VULKAN_ASSET_FIELD_DEFS(texture, texture)
}

void asset_texture_deinit(asset_texture *texture) {}

data_key asset_texture_calculate_key(asset_texture *texture) {
  hash_t value;
  HASH_START(hashState)
  if (texture->image) {
    HASH_UPDATE(hashState, &texture->image->key, sizeof(texture->image->key))
  }
  if (texture->sampler) {
    HASH_UPDATE(hashState, &texture->sampler->key, sizeof(texture->sampler->key))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_texture_serialize(asset_texture *texture, asset_db *assetDb) {
  texture->key = asset_texture_calculate_key(texture);

  asset_image_serialize(texture->image, assetDb);
  asset_db_insert_texture_image_key(assetDb, texture->key, texture->image->key);

  asset_sampler_serialize(texture->sampler, assetDb);
  asset_db_insert_texture_sampler_key(assetDb, texture->key, texture->sampler->key);
}

void asset_texture_deserialize(asset_texture *texture, asset_db *assetDb, data_key key) {
  texture->key = key;
  texture->image = scene_data_get_image_by_key(
      texture->sceneData, assetDb, asset_db_select_texture_image_key(assetDb, texture->key));
  texture->sampler = scene_data_get_sampler_by_key(
      texture->sceneData, assetDb, asset_db_select_texture_sampler_key(assetDb, texture->key));
}

void asset_texture_debug_print(asset_texture *texture, int indent) {
  log_debug(INDENT_FORMAT_STRING "texture:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), texture->key);
  asset_image_debug_print(texture->image, indent + 2);
  asset_sampler_debug_print(texture->sampler, indent + 2);
}

asset_texture *asset_texture_create_from_params(image_type type, uint32_t width, uint32_t height,
                                                uint32_t depth, uint32_t channels) {
  asset_texture *texture = core_alloc(sizeof(asset_texture));
  asset_texture_init(texture, NULL);

  asset_image *image = core_alloc(sizeof(asset_image));
  asset_image_init(image, NULL);
  image->width = width;
  image->height = height;
  image->depth = depth;
  image->channels = channels;
  image->type = type;
  texture->image = image;

  asset_sampler *sampler = core_alloc(sizeof(asset_sampler));
  asset_sampler_init(sampler, NULL);
  texture->sampler = sampler;

  return texture;
}

/* skybox */

void asset_skybox_init(asset_skybox *skybox, scene_data *sceneData) {
  utstring_new(skybox->name);
  skybox->cubemapTexture = NULL;

  VULKAN_ASSET_FIELD_DEFS(skybox, skybox)
}

void asset_skybox_deinit(asset_skybox *skybox) { utstring_free(skybox->name); }

data_key asset_skybox_calculate_key(asset_skybox *skybox) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(skybox->name), utstring_len(skybox->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_skybox_serialize(asset_skybox *skybox, asset_db *assetDb) {
  skybox->key = asset_skybox_calculate_key(skybox);

  asset_db_insert_skybox_name_text(assetDb, skybox->key, (data_text){skybox->name});
  asset_texture_serialize(skybox->cubemapTexture, assetDb);
  asset_db_insert_skybox_cubemapTexture_key(assetDb, skybox->key, skybox->cubemapTexture->key);
}

void asset_skybox_deserialize(asset_skybox *skybox, asset_db *assetDb, data_key key) {
  skybox->key = key;

  utstring_free(skybox->name);
  skybox->name = asset_db_select_skybox_name_text(assetDb, skybox->key).value;
  skybox->cubemapTexture = scene_data_get_texture_by_key(
      skybox->sceneData, assetDb, asset_db_select_skybox_cubemapTexture_key(assetDb, skybox->key));
}

void asset_skybox_debug_print(asset_skybox *skybox, int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), skybox->key);
  log_debug(INDENT_FORMAT_STRING "name=%s", INDENT_FORMAT_ARGS(2), utstring_body(skybox->name));
  asset_texture_debug_print(skybox->cubemapTexture, indent + 2);
}

/* font */

void asset_font_init(asset_font *font, scene_data *sceneData) {
  utstring_new(font->name);
  font->fontTexture = NULL;
  utstring_new(font->characters);
  font->characterSize = 0;

  VULKAN_ASSET_FIELD_DEFS(font, font)
}

void asset_font_deinit(asset_font *font) {
  utstring_free(font->name);
  utstring_free(font->characters);
}

data_key asset_font_calculate_key(asset_font *font) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(font->name), utstring_len(font->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_font_serialize(asset_font *font, asset_db *assetDb) {
  font->key = asset_font_calculate_key(font);

  asset_db_insert_font_name_text(assetDb, font->key, (data_text){font->name});
  asset_texture_serialize(font->fontTexture, assetDb);
  asset_db_insert_font_fontTexture_key(assetDb, font->key, font->fontTexture->key);
  asset_db_insert_font_characters_text(assetDb, font->key, (data_text){font->characters});
  asset_db_insert_font_characterSize_int(assetDb, font->key, data_int_temp(font->characterSize));
}

void asset_font_deserialize(asset_font *font, asset_db *assetDb, data_key key) {
  font->key = key;

  utstring_free(font->name);
  font->name = asset_db_select_font_name_text(assetDb, font->key).value;
  font->fontTexture = scene_data_get_texture_by_key(
      font->sceneData, assetDb, asset_db_select_font_fontTexture_key(assetDb, font->key));
  utstring_free(font->characters);
  font->characters = asset_db_select_font_characters_text(assetDb, font->key).value;
  font->characterSize = asset_db_select_font_characterSize_int(assetDb, font->key).value;
}

void asset_font_debug_print(asset_font *font, int indent) {
  log_debug(INDENT_FORMAT_STRING "font:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), font->key);
  log_debug(INDENT_FORMAT_STRING "name=%s", INDENT_FORMAT_ARGS(2), utstring_body(font->name));
  asset_texture_debug_print(font->fontTexture, indent + 2);
  log_debug(INDENT_FORMAT_STRING "characterSize=%u", INDENT_FORMAT_ARGS(2), font->characterSize);
}

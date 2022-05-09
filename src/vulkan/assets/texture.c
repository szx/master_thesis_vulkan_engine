#include "texture.h"
#include "../scene/data.h"

/* image */

void vulkan_asset_image_init(vulkan_asset_image *image, vulkan_scene_data *sceneData) {
  // NOTE: Default 1x1 RGBA image.
  image->width = 1;
  image->height = 1;
  image->depth = 1;
  image->channels = 4;
  image->faceCount = 1;
  image->type = vulkan_image_type_material_base_color;

  utarray_alloc(image->data, sizeof(uint8_t));
  utarray_resize(image->data, image->channels);
  for (size_t i = 0; i < utarray_len(image->data); i++) {
    *(uint8_t *)utarray_eltptr(image->data, i) = 255;
  }

  VULKAN_ASSET_FIELD_DEFS(image, image)
}

void vulkan_asset_image_deinit(vulkan_asset_image *image) { utarray_free(image->data); }

data_key vulkan_asset_image_calculate_key(vulkan_asset_image *image) {
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

void vulkan_asset_image_serialize(vulkan_asset_image *image, data_asset_db *assetDb) {
  image->key = vulkan_asset_image_calculate_key(image);
  data_asset_db_insert_image_width_int(assetDb, image->key, data_int_temp(image->width));
  data_asset_db_insert_image_height_int(assetDb, image->key, data_int_temp(image->height));
  data_asset_db_insert_image_depth_int(assetDb, image->key, data_int_temp(image->depth));
  data_asset_db_insert_image_channels_int(assetDb, image->key, data_int_temp(image->channels));
  data_asset_db_insert_image_faceCount_int(assetDb, image->key, data_int_temp(image->faceCount));
  data_asset_db_insert_image_type_int(assetDb, image->key, data_int_temp(image->type));
  data_asset_db_insert_image_data_byte_array(assetDb, image->key,
                                             data_byte_array_temp(image->data));
}

void vulkan_asset_image_deserialize(vulkan_asset_image *image, data_asset_db *assetDb,
                                    data_key key) {
  image->key = key;
  image->width = data_asset_db_select_image_width_int(assetDb, image->key).value;
  image->height = data_asset_db_select_image_height_int(assetDb, image->key).value;
  image->depth = data_asset_db_select_image_depth_int(assetDb, image->key).value;
  image->channels = data_asset_db_select_image_channels_int(assetDb, image->key).value;
  image->faceCount = data_asset_db_select_image_faceCount_int(assetDb, image->key).value;
  image->type = data_asset_db_select_image_type_int(assetDb, image->key).value;
  data_byte_array data = data_asset_db_select_image_data_byte_array(assetDb, image->key);
  utarray_resize(image->data, utarray_len(data.values));
  core_memcpy(image->data->d, data.values->d, utarray_size(image->data));
  data_byte_array_deinit(&data);
}

void vulkan_asset_image_debug_print(vulkan_asset_image *image, int indent) {
  log_debug(INDENT_FORMAT_STRING "image:\n", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), image->key);
  log_debug(INDENT_FORMAT_STRING "width=%zu", INDENT_FORMAT_ARGS(2), image->width);
  log_debug(INDENT_FORMAT_STRING "height=%zu", INDENT_FORMAT_ARGS(2), image->height);
  log_debug(INDENT_FORMAT_STRING "depth=%zu", INDENT_FORMAT_ARGS(2), image->depth);
  log_debug(INDENT_FORMAT_STRING "channels=%zu", INDENT_FORMAT_ARGS(2), image->channels);
  log_debug(INDENT_FORMAT_STRING "faceCount=%zu", INDENT_FORMAT_ARGS(2), image->faceCount);
  log_debug(INDENT_FORMAT_STRING "type=%s", INDENT_FORMAT_ARGS(2),
            vulkan_image_type_debug_str(image->type));
  log_debug(INDENT_FORMAT_STRING "data=%zu bytes", INDENT_FORMAT_ARGS(2), utarray_len(image->data));
}

/* sampler */

void vulkan_asset_sampler_init(vulkan_asset_sampler *sampler, vulkan_scene_data *sceneData) {
  sampler->magFilter = VK_FILTER_NEAREST;
  sampler->minFilter = VK_FILTER_NEAREST;
  sampler->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VULKAN_ASSET_FIELD_DEFS(sampler, sampler)
}

void vulkan_asset_sampler_deinit(vulkan_asset_sampler *sampler) {}

data_key vulkan_asset_sampler_calculate_key(vulkan_asset_sampler *sampler) {
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

void vulkan_asset_sampler_serialize(vulkan_asset_sampler *sampler, data_asset_db *assetDb) {
  sampler->key = vulkan_asset_sampler_calculate_key(sampler);
  data_asset_db_insert_sampler_magFilter_int(assetDb, sampler->key,
                                             data_int_temp(sampler->magFilter));
  data_asset_db_insert_sampler_minFilter_int(assetDb, sampler->key,
                                             data_int_temp(sampler->minFilter));
  data_asset_db_insert_sampler_addressWrapU_int(assetDb, sampler->key,
                                                data_int_temp(sampler->addressModeU));
  data_asset_db_insert_sampler_addressWrapV_int(assetDb, sampler->key,
                                                data_int_temp(sampler->addressModeV));
}

void vulkan_asset_sampler_deserialize(vulkan_asset_sampler *sampler, data_asset_db *assetDb,
                                      data_key key) {
  sampler->key = key;
  sampler->magFilter = data_asset_db_select_sampler_magFilter_int(assetDb, sampler->key).value;
  sampler->minFilter = data_asset_db_select_sampler_minFilter_int(assetDb, sampler->key).value;
  sampler->addressModeU =
      data_asset_db_select_sampler_addressWrapU_int(assetDb, sampler->key).value;
  sampler->addressModeV =
      data_asset_db_select_sampler_addressWrapV_int(assetDb, sampler->key).value;
}

void vulkan_asset_sampler_debug_print(vulkan_asset_sampler *sampler, int indent) {
  log_debug(INDENT_FORMAT_STRING "sampler:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), sampler->key);
  log_debug(INDENT_FORMAT_STRING "magFilter=%d", INDENT_FORMAT_ARGS(2), sampler->magFilter);
  log_debug(INDENT_FORMAT_STRING "minFilter=%d", INDENT_FORMAT_ARGS(2), sampler->minFilter);
  log_debug(INDENT_FORMAT_STRING "addressWrapU=%d", INDENT_FORMAT_ARGS(2), sampler->addressModeU);
  log_debug(INDENT_FORMAT_STRING "addressWrapV=%d", INDENT_FORMAT_ARGS(2), sampler->addressModeV);
}

/* texture */

void vulkan_asset_texture_init(vulkan_asset_texture *texture, vulkan_scene_data *sceneData) {
  texture->image = NULL;
  texture->sampler = NULL;

  VULKAN_ASSET_FIELD_DEFS(texture, texture)
}

void vulkan_asset_texture_deinit(vulkan_asset_texture *texture) {}

data_key vulkan_asset_texture_calculate_key(vulkan_asset_texture *texture) {
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

void vulkan_asset_texture_serialize(vulkan_asset_texture *texture, data_asset_db *assetDb) {
  texture->key = vulkan_asset_texture_calculate_key(texture);

  vulkan_asset_image_serialize(texture->image, assetDb);
  data_asset_db_insert_texture_image_key(assetDb, texture->key, texture->image->key);

  vulkan_asset_sampler_serialize(texture->sampler, assetDb);
  data_asset_db_insert_texture_sampler_key(assetDb, texture->key, texture->sampler->key);
}

void vulkan_asset_texture_deserialize(vulkan_asset_texture *texture, data_asset_db *assetDb,
                                      data_key key) {
  texture->key = key;
  texture->image = vulkan_scene_data_get_image_by_key(
      texture->sceneData, assetDb, data_asset_db_select_texture_image_key(assetDb, texture->key));
  texture->sampler = vulkan_scene_data_get_sampler_by_key(
      texture->sceneData, assetDb, data_asset_db_select_texture_sampler_key(assetDb, texture->key));
}

void vulkan_asset_texture_debug_print(vulkan_asset_texture *texture, int indent) {
  log_debug(INDENT_FORMAT_STRING "texture:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), texture->key);
  vulkan_asset_image_debug_print(texture->image, indent + 2);
  vulkan_asset_sampler_debug_print(texture->sampler, indent + 2);
}

/* skybox */

void vulkan_asset_skybox_init(vulkan_asset_skybox *skybox, vulkan_scene_data *sceneData) {
  utstring_new(skybox->name);
  skybox->cubemapTexture = NULL;

  VULKAN_ASSET_FIELD_DEFS(skybox, skybox)
}

void vulkan_asset_skybox_deinit(vulkan_asset_skybox *skybox) { utstring_free(skybox->name); }

data_key vulkan_asset_skybox_calculate_key(vulkan_asset_skybox *skybox) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(skybox->name), utstring_len(skybox->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_asset_skybox_serialize(vulkan_asset_skybox *skybox, data_asset_db *assetDb) {
  skybox->key = vulkan_asset_skybox_calculate_key(skybox);

  data_asset_db_insert_skybox_name_text(assetDb, skybox->key, (data_text){skybox->name});
  vulkan_asset_texture_serialize(skybox->cubemapTexture, assetDb);
  data_asset_db_insert_skybox_cubemapTexture_key(assetDb, skybox->key, skybox->cubemapTexture->key);
}

void vulkan_asset_skybox_deserialize(vulkan_asset_skybox *skybox, data_asset_db *assetDb,
                                     data_key key) {
  skybox->key = key;

  utstring_free(skybox->name);
  skybox->name = data_asset_db_select_skybox_name_text(assetDb, skybox->key).value;
  skybox->cubemapTexture = vulkan_scene_data_get_texture_by_key(
      skybox->sceneData, assetDb,
      data_asset_db_select_skybox_cubemapTexture_key(assetDb, skybox->key));
}

void vulkan_asset_skybox_debug_print(vulkan_asset_skybox *skybox, int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), skybox->key);
  log_debug(INDENT_FORMAT_STRING "name=%s", INDENT_FORMAT_ARGS(2), utstring_body(skybox->name));
  vulkan_asset_texture_debug_print(skybox->cubemapTexture, indent + 2);
}

#include "texture.h"
#include "scene.h"

/* image */

void vulkan_data_image_init(vulkan_data_image *image) {
  image->width = 0;
  image->height = 0;
  image->depth = 0;
  image->channels = 0;
  utarray_alloc(image->data, sizeof(uint8_t));

  image->hash = vulkan_data_image_calculate_key(image);
  image->prev = NULL;
  image->next = NULL;
}

void vulkan_data_image_deinit(vulkan_data_image *image) { utarray_free(image->data); }

data_key vulkan_data_image_calculate_key(vulkan_data_image *image) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &image->width, sizeof(image->width));
  HASH_UPDATE(hashState, &image->height, sizeof(image->height));
  HASH_UPDATE(hashState, &image->depth, sizeof(image->depth));
  HASH_UPDATE(hashState, &image->channels, sizeof(image->channels));
  HASH_UPDATE(hashState, utarray_front(image->data), utarray_size(image->data))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_image_serialize(vulkan_data_image *image, data_asset_db *assetDb) {
  image->hash = vulkan_data_image_calculate_key(image);
  data_asset_db_insert_image_width_int(assetDb, image->hash, data_int_temp(image->width));
  data_asset_db_insert_image_height_int(assetDb, image->hash, data_int_temp(image->height));
  data_asset_db_insert_image_depth_int(assetDb, image->hash, data_int_temp(image->depth));
  data_asset_db_insert_image_channels_int(assetDb, image->hash, data_int_temp(image->channels));
  data_asset_db_insert_image_data_byte_array(assetDb, image->hash,
                                             data_byte_array_temp(image->data));
}

void vulkan_data_image_deserialize(vulkan_data_image *image, data_asset_db *assetDb, data_key key) {
  image->hash = key;
  data_byte_array data = data_asset_db_select_image_data_byte_array(assetDb, image->hash);
  utarray_resize(image->data, utarray_len(data.values));
  core_memcpy(image->data->d, data.values->d, utarray_size(image->data));
  data_byte_array_deinit(&data);
}

void vulkan_data_image_debug_print(vulkan_data_image *image) {
  log_debug("image:");
  log_debug("  hash=%zu", image->hash);
  log_debug("  data=%zu bytes", utarray_len(image->data));
}

/* sampler */

void vulkan_data_sampler_init(vulkan_data_sampler *sampler) {
  sampler->magFilter = VK_FILTER_NEAREST;
  sampler->minFilter = VK_FILTER_NEAREST;
  sampler->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  sampler->hash = vulkan_data_sampler_calculate_key(sampler);
  sampler->prev = NULL;
  sampler->next = NULL;
}

void vulkan_data_sampler_deinit(vulkan_data_sampler *sampler) {}

data_key vulkan_data_sampler_calculate_key(vulkan_data_sampler *sampler) {
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

void vulkan_data_sampler_serialize(vulkan_data_sampler *sampler, data_asset_db *assetDb) {
  sampler->hash = vulkan_data_sampler_calculate_key(sampler);
  data_asset_db_insert_sampler_magFilter_int(assetDb, sampler->hash,
                                             data_int_temp(sampler->magFilter));
  data_asset_db_insert_sampler_minFilter_int(assetDb, sampler->hash,
                                             data_int_temp(sampler->minFilter));
  data_asset_db_insert_sampler_addressWrapU_int(assetDb, sampler->hash,
                                                data_int_temp(sampler->addressModeU));
  data_asset_db_insert_sampler_addressWrapV_int(assetDb, sampler->hash,
                                                data_int_temp(sampler->addressModeV));
}

void vulkan_data_sampler_deserialize(vulkan_data_sampler *sampler, data_asset_db *assetDb,
                                     data_key key) {
  sampler->hash = key;
  sampler->magFilter = data_asset_db_select_sampler_magFilter_int(assetDb, sampler->hash).value;
  sampler->minFilter = data_asset_db_select_sampler_minFilter_int(assetDb, sampler->hash).value;
  sampler->addressModeU =
      data_asset_db_select_sampler_addressWrapU_int(assetDb, sampler->hash).value;
  sampler->addressModeV =
      data_asset_db_select_sampler_addressWrapV_int(assetDb, sampler->hash).value;
}

void vulkan_data_sampler_debug_print(vulkan_data_sampler *sampler) {
  log_debug("sampler:");
  log_debug("  hash=%zu", sampler->hash);
  log_debug("  magFilter=%d", sampler->magFilter);
  log_debug("  minFilter=%d", sampler->minFilter);
  log_debug("  addressWrapU=%d", sampler->addressModeU);
  log_debug("  addressWrapV=%d", sampler->addressModeV);
}

/* texture */

void vulkan_data_texture_init(vulkan_data_texture *texture, vulkan_data_scene *sceneData) {
  texture->sceneData = sceneData;
  texture->image = NULL;
  texture->sampler = NULL;

  data_key_init(&texture->hash, 0);
  texture->prev = NULL;
  texture->next = NULL;
}

void vulkan_data_texture_deinit(vulkan_data_texture *texture) {}

data_key vulkan_data_texture_calculate_key(vulkan_data_texture *texture) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &texture->image->hash, sizeof(texture->image->hash))
  HASH_UPDATE(hashState, &texture->sampler->hash, sizeof(texture->sampler->hash))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_texture_serialize(vulkan_data_texture *texture, data_asset_db *assetDb) {
  texture->hash = vulkan_data_texture_calculate_key(texture);

  vulkan_data_image_serialize(texture->image, assetDb);
  data_asset_db_insert_texture_image_key(assetDb, texture->hash, texture->image->hash);

  vulkan_data_sampler_serialize(texture->sampler, assetDb);
  data_asset_db_insert_texture_sampler_key(assetDb, texture->hash, texture->sampler->hash);
}

void vulkan_data_texture_deserialize(vulkan_data_texture *texture, data_asset_db *assetDb,
                                     data_key key) {
  texture->hash = key;
  texture->image = vulkan_data_scene_get_image_by_key(
      texture->sceneData, assetDb, data_asset_db_select_texture_image_key(assetDb, texture->hash));
  texture->sampler = vulkan_data_scene_get_sampler_by_key(
      texture->sceneData, assetDb,
      data_asset_db_select_texture_sampler_key(assetDb, texture->hash));
}

void vulkan_data_texture_debug_print(vulkan_data_texture *texture) {
  log_debug("texture:");
  log_debug("  hash=%zu", texture->hash);
  vulkan_data_image_debug_print(texture->image);
  vulkan_data_sampler_debug_print(texture->sampler);
}

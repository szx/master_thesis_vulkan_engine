/* Texture state.
 * Represents PBR parameters of a primitive.
 * Part of a material.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

/* image */

typedef struct vulkan_image_data {
  uint32_t width, height, depth, channels;
  UT_array *data; // uint8_t array

  data_key hash; /// Hash, used to prevent duplicates in asset database.
  struct vulkan_image_data *prev, *next;
} vulkan_image_data;

void vulkan_image_data_init(vulkan_image_data *image);
void vulkan_image_data_deinit(vulkan_image_data *image);

data_key vulkan_image_data_calculate_key(vulkan_image_data *image);
void vulkan_image_data_serialize(vulkan_image_data *image, data_asset_db *assetDb);
void vulkan_image_data_deserialize(vulkan_image_data *image, data_asset_db *assetDb, data_key key);

void vulkan_image_data_debug_print(vulkan_image_data *sampler);

/* sampler */

typedef struct vulkan_sampler_data {
  VkFilter magFilter, minFilter;
  VkSamplerAddressMode addressModeU, addressModeV;

  data_key hash; /// Hash, used to prevent duplicates in asset database.
  struct vulkan_sampler_data *prev, *next;
} vulkan_sampler_data;

void vulkan_sampler_data_init(vulkan_sampler_data *sampler);
void vulkan_sampler_data_deinit(vulkan_sampler_data *sampler);

data_key vulkan_sampler_data_calculate_key(vulkan_sampler_data *sampler);
void vulkan_sampler_data_serialize(vulkan_sampler_data *sampler, data_asset_db *assetDb);
void vulkan_sampler_data_deserialize(vulkan_sampler_data *sampler, data_asset_db *assetDb,
                                     data_key key);

void vulkan_sampler_data_debug_print(vulkan_sampler_data *sampler);

/* texture */

typedef struct vulkan_scene_data vulkan_scene_data;

typedef struct vulkan_texture_data {
  vulkan_scene_data *sceneData; /// vulkan_scene_data pointer
  vulkan_image_data *image;
  vulkan_sampler_data *sampler;

  data_key hash; /// Hash, used to prevent duplicates in asset database.
  struct vulkan_texture_data *prev, *next;
} vulkan_texture_data;

void vulkan_texture_data_init(vulkan_texture_data *texture, vulkan_scene_data *sceneData);
void vulkan_texture_data_deinit(vulkan_texture_data *texture);

data_key vulkan_texture_data_calculate_key(vulkan_texture_data *texture);
void vulkan_texture_data_serialize(vulkan_texture_data *texture, data_asset_db *assetDb);
void vulkan_texture_data_deserialize(vulkan_texture_data *texture, data_asset_db *assetDb,
                                     data_key key);

void vulkan_texture_data_debug_print(vulkan_texture_data *texture);

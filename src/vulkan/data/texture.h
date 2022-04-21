/* Texture state.
 * Represents PBR parameters of a primitive.
 * Part of a material.
 */
#pragma once

#include "common.h"

/* image */

typedef enum vulkan_data_image_type {
  vulkan_data_image_type_material_base_color,
  vulkan_data_image_type_material_parameters,
  vulkan_data_image_type_count
} vulkan_data_image_type;

typedef struct vulkan_data_image {
  uint32_t width, height, depth, channels;
  vulkan_data_image_type type;
  UT_array *data; // uint8_t array

  DECL_VULKAN_ENTITY(image)
} vulkan_data_image;

void vulkan_data_image_init(vulkan_data_image *image, vulkan_data_scene *sceneData);
void vulkan_data_image_deinit(vulkan_data_image *image);

data_key vulkan_data_image_calculate_key(vulkan_data_image *image);
void vulkan_data_image_serialize(vulkan_data_image *image, data_asset_db *assetDb);
void vulkan_data_image_deserialize(vulkan_data_image *image, data_asset_db *assetDb, data_key key);

void vulkan_data_image_debug_print(vulkan_data_image *sampler);

/* sampler */

typedef struct vulkan_data_sampler {
  VkFilter magFilter, minFilter;
  VkSamplerAddressMode addressModeU, addressModeV;

  DECL_VULKAN_ENTITY(sampler)
} vulkan_data_sampler;

void vulkan_data_sampler_init(vulkan_data_sampler *sampler, vulkan_data_scene *sceneData);
void vulkan_data_sampler_deinit(vulkan_data_sampler *sampler);

data_key vulkan_data_sampler_calculate_key(vulkan_data_sampler *sampler);
void vulkan_data_sampler_serialize(vulkan_data_sampler *sampler, data_asset_db *assetDb);
void vulkan_data_sampler_deserialize(vulkan_data_sampler *sampler, data_asset_db *assetDb,
                                     data_key key);

void vulkan_data_sampler_debug_print(vulkan_data_sampler *sampler);

/* texture */

typedef struct vulkan_data_scene vulkan_data_scene;

typedef struct vulkan_data_texture {
  vulkan_data_image *image;
  vulkan_data_sampler *sampler;

  DECL_VULKAN_ENTITY(texture)
} vulkan_data_texture;

void vulkan_data_texture_init(vulkan_data_texture *texture, vulkan_data_scene *sceneData);
void vulkan_data_texture_deinit(vulkan_data_texture *texture);

data_key vulkan_data_texture_calculate_key(vulkan_data_texture *texture);
void vulkan_data_texture_serialize(vulkan_data_texture *texture, data_asset_db *assetDb);
void vulkan_data_texture_deserialize(vulkan_data_texture *texture, data_asset_db *assetDb,
                                     data_key key);

void vulkan_data_texture_debug_print(vulkan_data_texture *texture);

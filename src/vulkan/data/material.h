/* Material state.
 * Represents PBR parameters of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_material_data {
  vec4 baseColorFactor; /// sRGBA
  float metallicFactor;
  float roughnessFactor;
  // HIRO material textures
  data_key hash; /// Hash, used to prevent duplicates in asset database.
  struct vulkan_material_data *prev;
  struct vulkan_material_data *next;
} vulkan_material_data;

void vulkan_material_data_init(vulkan_material_data *material);
void vulkan_material_data_deinit(vulkan_material_data *material);

data_key vulkan_material_data_calculate_key(vulkan_material_data *material);
void vulkan_material_data_serialize(vulkan_material_data *material, data_asset_db *assetDb);
void vulkan_material_data_deserialize(vulkan_material_data *material, data_asset_db *assetDb,
                                      data_key key);

void vulkan_material_data_debug_print(vulkan_material_data *material);

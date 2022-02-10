/* Material state.
 * Represents PBR parameters of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "common.h"
#include "texture.h"

typedef struct vulkan_scene_node vulkan_scene_node;

typedef struct vulkan_data_material {
  vec4 baseColorFactor;         /// sRGBA
  float metallicFactor;
  float roughnessFactor;
  vulkan_data_texture *baseColorTexture;
  vulkan_data_texture *metallicRoughnessTexture;

  DECL_VULKAN_ENTITY(material)
} vulkan_data_material;

void vulkan_data_material_init(vulkan_data_material *material, vulkan_data_scene *sceneData);
void vulkan_data_material_deinit(vulkan_data_material *material);

data_key vulkan_data_material_calculate_key(vulkan_data_material *material);
void vulkan_data_material_serialize(vulkan_data_material *material, data_asset_db *assetDb);
void vulkan_data_material_deserialize(vulkan_data_material *material, data_asset_db *assetDb,
                                      data_key key);

void vulkan_data_material_debug_print(vulkan_data_material *material);

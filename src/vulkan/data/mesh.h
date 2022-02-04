/* Mesh state.
 * Represents a group of primitives.
 * Used to:
 * store all geometry of one node
 * cull nodes
 * TODO: skinning
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_scene_data vulkan_scene_data;

typedef struct vulkan_mesh_data {
  vulkan_scene_data *sceneData; // vulkan_scene_data pointer
  UT_array *primitives;         /// vulkan_primitive_data_index array.
  data_key hash;                /// Hash, used to prevent duplicates in asset database.
} vulkan_mesh_data;

void vulkan_mesh_data_init(vulkan_mesh_data *mesh, vulkan_scene_data *sceneData);
void vulkan_mesh_data_deinit(vulkan_mesh_data *mesh);

data_key vulkan_mesh_data_calculate_key(vulkan_mesh_data *mesh);
void vulkan_mesh_data_serialize(vulkan_mesh_data *mesh, data_asset_db *assetDb);
void vulkan_mesh_data_deserialize(vulkan_mesh_data *mesh, data_asset_db *assetDb, data_key key);

void vulkan_mesh_data_debug_print(vulkan_mesh_data *mesh);

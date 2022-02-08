/* Mesh state.
 * Represents a group of primitives.
 * Used to:
 * store all geometry of one object,
 * calculate culling boundaries of objects.
 * TODO: skinning
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_data_scene vulkan_data_scene;

typedef struct vulkan_data_mesh {
  vulkan_data_scene *sceneData; // vulkan_data_scene pointer
  UT_array *primitives;         /// vulkan_data_primitive_index array.

  data_key hash;                /// Hash, used to prevent duplicates in asset database.
} vulkan_data_mesh;

void vulkan_data_mesh_init(vulkan_data_mesh *mesh, vulkan_data_scene *sceneData);
void vulkan_data_mesh_deinit(vulkan_data_mesh *mesh);

data_key vulkan_data_mesh_calculate_key(vulkan_data_mesh *mesh);
void vulkan_data_mesh_serialize(vulkan_data_mesh *mesh, data_asset_db *assetDb);
void vulkan_data_mesh_deserialize(vulkan_data_mesh *mesh, data_asset_db *assetDb, data_key key);

void vulkan_data_mesh_debug_print(vulkan_data_mesh *mesh, int indent);

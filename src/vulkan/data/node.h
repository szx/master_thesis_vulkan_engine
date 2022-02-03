/* Node state.
 * Represents one object in scene.
 * Contains one mesh and transform.
 * Used to TODO
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"
#include "mesh.h"

typedef struct vulkan_node_data {
  vulkan_scene_data *sceneData; // vulkan_scene_data pointer
  vulkan_mesh_data mesh;
  mat4 transform;
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_node_data;

void vulkan_node_data_init(vulkan_node_data *node, vulkan_scene_data *sceneData);
void vulkan_node_data_deinit(vulkan_node_data *node);

data_key vulkan_node_data_calculate_key(vulkan_node_data *node);
void vulkan_node_data_serialize(vulkan_node_data *node, data_asset_db *assetDb);
void vulkan_node_data_deserialize(vulkan_node_data *node, data_asset_db *assetDb, data_key key);

void vulkan_node_data_debug_print(vulkan_node_data *node);

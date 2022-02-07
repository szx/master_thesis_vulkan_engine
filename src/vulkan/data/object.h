/* Node state.
 * Represents one object in scene.
 * Contains one mesh and transform.
 * Used to TODO
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"
#include "mesh.h"

typedef struct vulkan_data_node {
  vulkan_data_scene *sceneData; // vulkan_data_scene pointer
  vulkan_data_mesh mesh;
  mat4 transform;
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_data_node;

void vulkan_data_node_init(vulkan_data_node *node, vulkan_data_scene *sceneData);
void vulkan_data_node_deinit(vulkan_data_node *node);

data_key vulkan_data_node_calculate_key(vulkan_data_node *node);
void vulkan_data_node_serialize(vulkan_data_node *node, data_asset_db *assetDb);
void vulkan_data_node_deserialize(vulkan_data_node *node, data_asset_db *assetDb, data_key key);

void vulkan_data_node_debug_print(vulkan_data_node *node);

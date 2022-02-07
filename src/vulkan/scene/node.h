/* Vulkan scene node.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to:
 * build and synchronize scene tree.
 */
#pragma once

#include "../data/scene.h"

// HIRO Enum with type? (primitive, mesh, transform, object)
typedef struct vulkan_scene_node {
  vulkan_data_scene *data; /// vulkan_data_scene pointer.
  // HIRO scene_node_nodes
} vulkan_scene_node;

vulkan_scene_node *vulkan_scene_node_create(vulkan_data_scene *sceneData);
void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode);

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode);

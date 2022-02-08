/* Vulkan scene graph.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to:
 * build and synchronize scene tree.
 */
#pragma once

#include "node.h"

typedef struct vulkan_scene_graph {
  vulkan_data_scene *data; /// vulkan_data_scene pointer.

  // HIRO scene_graph_nodes
  vulkan_scene_node *root;
} vulkan_scene_graph;

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *sceneData);
void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph);

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData);

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph);

/* Vulkan scene graph.
 * Used to build scene tree.
 */
#pragma once

#include "../data/scene.h"
#include "../device.h"
#include "../functions.h"
#include "../geometry_buffer.h"

// HIRO SceneGraph, SceneTree, ShapeList, Batches?
typedef struct vulkan_scene_graph {
  vulkan_data_scene *data; /// vulkan_data_scene pointer.
  // HIRO scene_graph_nodes
} vulkan_scene_graph;

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *sceneData);
void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph);

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph);

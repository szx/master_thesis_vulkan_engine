/* Vulkan scene graph.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to build and synchronize scene tree.
 */
#pragma once

#include "node.h"

typedef struct vulkan_scene_tree vulkan_scene_tree;

typedef struct vulkan_scene_graph {
  vulkan_data_scene *data;      /// vulkan_data_scene pointer.
  vulkan_scene_tree *sceneTree; /// Scene tree.
  // HIRO Scene render data.

  vulkan_scene_node *root; /// Both root node and list of all nodes of scene graph.
} vulkan_scene_graph;

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *sceneData);
void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph);

/// Adds object data as new scene node.
vulkan_scene_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                 vulkan_scene_node *sceneNode,
                                                 vulkan_data_object *successorObject);

/// Removes all instances of scene node from scene graph.
void vulkan_scene_graph_remove_object(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneNode);

void vulkan_scene_graph_set_dirty(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneNode);

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData);

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph);

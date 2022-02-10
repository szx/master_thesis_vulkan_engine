/* Vulkan scene tree.
 * Contains scene nodes created from scene graph.
 * Used to build and synchronize scene render data.
 */
#pragma once

#include "graph.h"

// HIRO add invalidation
// HIRO add adding/removing nodes from scene graph
//  - Scene graph set_dirty() sets observers as dirty and adds to scene tree's dirty vector.

typedef struct vulkan_scene_tree {
  vulkan_scene_graph *graph; /// vulkan_data_scene pointer.

  vulkan_scene_node *root;  /// Root node.
  vulkan_scene_node *nodes; /// Doubly-linked list of all nodes of scene graph.
  // HIRO dirty nodes array
} vulkan_scene_tree;

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph);
void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree);

void vulkan_scene_tree_create_with_scene_graph(vulkan_scene_tree *sceneTree,
                                               vulkan_scene_graph *sceneGraph);

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree);

/* Vulkan scene tree.
 * Contains scene nodes created from scene graph.
 * Used to build and synchronize scene render data.
 */
#pragma once

#include "graph.h"

// HIRO add adding/removing nodes from scene graph

typedef struct vulkan_scene_tree {
  vulkan_scene_graph *graph; /// vulkan_scene_graph pointer.

  vulkan_scene_node *root;  /// Root node.
  vulkan_scene_node *nodes; /// Doubly-linked list of all nodes of scene tree.
  UT_array *dirtyNodes;     /// vulkan_scene_node* array of dirty scene tree nodes.
} vulkan_scene_tree;

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph);
void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree);

void vulkan_scene_tree_set_dirty(vulkan_scene_tree *sceneTree, vulkan_scene_node *sceneNode);
void vulkan_scene_tree_validate(vulkan_scene_tree *sceneTree);

void vulkan_scene_tree_create_with_scene_graph(vulkan_scene_tree *sceneTree,
                                               vulkan_scene_graph *sceneGraph);

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree);

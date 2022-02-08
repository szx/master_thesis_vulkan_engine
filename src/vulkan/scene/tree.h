/* Vulkan scene tree.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to:
 * build and synchronize scene tree.
 */
#pragma once

#include "node.h"

// HIRO implement scene tree from scene graph

typedef struct vulkan_scene_tree {
  vulkan_data_scene *data; /// vulkan_data_scene pointer.

  vulkan_data_object *rootObject; /// Object data of root node.
  vulkan_scene_node *root;        /// Root node and doubly-linked list of all nodes to scene tree.
} vulkan_scene_tree;

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_data_scene *sceneData);
void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneGraph);

void vulkan_scene_tree_create_with_scene_data(vulkan_scene_tree *sceneGraph,
                                              vulkan_data_scene *sceneData);

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneGraph);

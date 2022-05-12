/* Vulkan scene tree.
 * Contains scene nodes created from scene graph.
 * Used to build and synchronize scene render data.
 */
#pragma once

#include "graph.h"

typedef struct vulkan_renderer_cache vulkan_renderer_cache;

typedef struct vulkan_scene_tree {
  vulkan_scene_graph *graph;            ///< Pointer.
  vulkan_renderer_cache *rendererCache; ///< Pointer. Tree adds cache for each primitive node leaf.

  vulkan_scene_tree_node *root;  ///< Root node.
  vulkan_scene_tree_node *nodes; ///< Doubly-linked list of all nodes of scene tree.
  UT_array *dirtyNodes;          ///< vulkan_scene_tree_node* array of dirty scene tree nodes.
} vulkan_scene_tree;

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph,
                                            vulkan_renderer_cache *rendererCache);
void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree);

/// Adds scene entity data as new tree node.
vulkan_scene_tree_node *vulkan_scene_tree_add_node(vulkan_scene_tree *sceneTree,
                                                   vulkan_scene_graph_node *sceneGraphNode,
                                                   vulkan_scene_graph_node *parentSceneGraphNode,
                                                   vulkan_scene_tree_node *parentSceneTreeNode);

void vulkan_scene_tree_set_dirty(vulkan_scene_tree *sceneTree,
                                 vulkan_scene_tree_node *sceneTreeNode);
void vulkan_scene_tree_validate(vulkan_scene_tree *sceneTree);

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree);

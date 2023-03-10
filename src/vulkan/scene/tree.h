/* Vulkan scene tree.
 * Contains scene nodes created from scene graph.
 * Used to build and synchronize scene render data.
 */
#pragma once

#include "graph.h"

typedef struct renderer_cache renderer_cache;

typedef struct scene_tree {
  scene_graph *graph;            ///< Pointer.
  renderer_cache *rendererCache; ///< Pointer. Tree adds cache for each primitive node leaf.

  scene_tree_node *root;  ///< Root node.
  scene_tree_node *nodes; ///< Doubly-linked list of all nodes of scene tree.
  UT_array *dirtyNodes;   ///< scene_tree_node* array of dirty scene tree nodes.
} scene_tree;

scene_tree *scene_tree_create(scene_graph *sceneGraph, renderer_cache *rendererCache);
void scene_tree_destroy(scene_tree *sceneTree);

/// Adds scene entity data as new tree node.
scene_tree_node *scene_tree_add_node(scene_tree *sceneTree, scene_graph_node *sceneGraphNode,
                                     scene_graph_node *parentSceneGraphNode,
                                     scene_tree_node *parentSceneTreeNode);

void scene_tree_set_dirty(scene_tree *sceneTree, scene_tree_node *sceneTreeNode);
void scene_tree_validate(scene_tree *sceneTree);

void scene_tree_debug_print(scene_tree *sceneTree);

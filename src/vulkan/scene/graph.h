/* Vulkan scene graph.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to build and synchronize scene tree.
 */
#pragma once

#include "data.h"
#include "node.h"

typedef struct renderer_cache renderer_cache;
typedef struct scene_tree scene_tree;

typedef struct scene_graph {
  scene_data *data;      ///< scene_data pointer.
  scene_tree *sceneTree; ///< Scene tree.

  scene_graph_node *root; ///< The root node.

  scene_graph_node *nodes; ///< Linked list of all nodes of scene graph.
} scene_graph;

scene_graph *scene_graph_create(scene_data *sceneData, renderer_cache *rendererCache);
void scene_graph_destroy(scene_graph *sceneGraph);

/// Adds object data as new scene graph and tree nodes.
scene_graph_node *scene_graph_add_object(scene_graph *sceneGraph, scene_graph_node *sceneGraphNode,
                                         scene_tree_node *sceneTreeNode,
                                         asset_object *successorObject);

void scene_graph_set_dirty(scene_graph *sceneGraph, scene_graph_node *sceneGraphNode);

void scene_graph_create_with_scene_data(scene_graph *sceneGraph, scene_data *sceneData);

void scene_graph_debug_print(scene_graph *sceneGraph);

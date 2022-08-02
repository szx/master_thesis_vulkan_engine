/* Vulkan scene node.
 * Contains pointers to entities in scene data and renderer cache elements.
 * Used to:
 * build scene graph with nodes corresponding 1:1 to scene data,
 * build scene tree with nodes from scene graph,
 * build render data with scene tree primitive nodes.
 */
#pragma once

#include "../assets/assets.h"
#include "../renderers/renderer_cache_elements.h"

typedef struct scene_graph scene_graph;
typedef struct scene_tree scene_tree;

/* scene graph node */

typedef struct scene_tree_node scene_tree_node;

typedef struct scene_graph_node {
  scene_graph *sceneGraph; ///< Pointer

  asset_object *object;       ///< Pointer.
  asset_primitive *primitive; ///< Pointer.

  UT_array *childNodes; ///< scene_graph_node* list

  struct scene_graph_node *prev, *next; ///< List of all nodes in scene graph.

  UT_array *observers; ///< scene_tree_node* list of dependent scene tree nodes
} scene_graph_node;

scene_graph_node *scene_graph_node_create(scene_graph *sceneGraph, asset_object *object,
                                          asset_primitive *primitive);

void scene_graph_node_destroy(scene_graph_node *sceneGraphNode);

void scene_graph_node_add_observer(scene_graph_node *sceneGraphNode,
                                   scene_tree_node *sceneTreeNode);

void scene_graph_node_add_child(scene_graph_node *sceneGraphNode, scene_graph_node *childNode);

void scene_graph_node_debug_print(scene_graph_node *sceneGraphNode);

/* scene tree node */

typedef struct scene_tree_node_accumulated {
  /* accumulated from object */
  size_t distanceFromRoot;
  bool visible;
  mat4 transform;

  /* accumulated from primitive */
  asset_primitive *primitive;
  aabb aabb;

  /* accumulated from camera */
  asset_camera *camera;
} scene_tree_node_accumulated;

void scene_tree_node_accumulated_init(scene_tree_node_accumulated *accumulated);
void scene_tree_node_accumulated_deinit(scene_tree_node_accumulated *accumulated);
void scene_tree_node_accumulated_debug_print(scene_tree_node_accumulated *accumulated);

typedef struct scene_tree_node {
  scene_tree *sceneTree; ///< Pointer

  asset_object *object;       ///< Pointer.
  asset_primitive *primitive; ///< Pointer.

  struct scene_tree_node *parentNode;
  UT_array *childNodes; ///< scene_tree_node* list

  struct scene_tree_node *prev, *next; ///< List of all nodes in scene tree.

  scene_tree_node_accumulated accumulated;

  bool dirty; ///< True if scene node state changed and cache is out of sync.
} scene_tree_node;

scene_tree_node *scene_tree_node_create(scene_tree *sceneTree, scene_graph_node *sceneGraphNode);

void scene_tree_node_destroy(scene_tree_node *sceneTreeNode);

void scene_tree_node_add_parent(scene_tree_node *sceneTreeNode, scene_tree_node *parentNode);

void scene_tree_node_add_child(scene_tree_node *sceneTreeNode, scene_tree_node *childNode);

void scene_tree_node_reset_accumulated(scene_tree_node *sceneTreeNode);
void scene_tree_node_accumulate_from_parent(scene_tree_node *sceneTreeNode);

void scene_tree_node_debug_print(scene_tree_node *sceneTreeNode);

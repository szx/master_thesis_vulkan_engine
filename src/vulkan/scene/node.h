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

typedef struct vulkan_scene_graph vulkan_scene_graph;
typedef struct vulkan_scene_tree vulkan_scene_tree;

/* scene graph node */

typedef struct vulkan_scene_tree_node vulkan_scene_tree_node;

typedef struct vulkan_scene_graph_node {
  vulkan_scene_graph *sceneGraph; ///< Pointer

  vulkan_asset_object *object;       ///< Pointer.
  vulkan_asset_primitive *primitive; ///< Pointer.

  UT_array *childNodes; ///< vulkan_scene_graph_node* list

  struct vulkan_scene_graph_node *prev, *next; ///< List of all nodes in scene graph.

  UT_array *observers; ///< vulkan_scene_tree_node* list of dependent scene tree nodes
} vulkan_scene_graph_node;

vulkan_scene_graph_node *vulkan_scene_graph_node_create(vulkan_scene_graph *sceneGraph,
                                                        vulkan_asset_object *object,
                                                        vulkan_asset_primitive *primitive);

void vulkan_scene_graph_node_destroy(vulkan_scene_graph_node *sceneGraphNode);

void vulkan_scene_graph_node_add_observer(vulkan_scene_graph_node *sceneGraphNode,
                                          vulkan_scene_tree_node *sceneTreeNode);

void vulkan_scene_graph_node_add_child(vulkan_scene_graph_node *sceneGraphNode,
                                       vulkan_scene_graph_node *childNode);

void vulkan_scene_graph_node_debug_print(vulkan_scene_graph_node *sceneGraphNode);

/* scene tree node */

typedef struct vulkan_scene_tree_node {
  vulkan_scene_tree *sceneTree; ///< Pointer

  vulkan_asset_object *object;       ///< Pointer.
  vulkan_asset_primitive *primitive; ///< Pointer.

  struct vulkan_scene_tree_node *parentNode;
  UT_array *childNodes; ///< vulkan_scene_tree_node* list

  struct vulkan_scene_tree_node *prev, *next; ///< List of all nodes in scene tree.

  vulkan_renderer_cache_primitive_element *primitiveElement;
  vulkan_renderer_cache_camera_element *cameraElement;

  bool dirty; ///< True if scene node state changed and cache is out of sync.
} vulkan_scene_tree_node;

vulkan_scene_tree_node *vulkan_scene_tree_node_create(vulkan_scene_tree *sceneTree,
                                                      vulkan_scene_graph_node *sceneGraphNode);

void vulkan_scene_tree_node_destroy(vulkan_scene_tree_node *sceneTreeNode);

void vulkan_scene_tree_node_add_parent(vulkan_scene_tree_node *sceneTreeNode,
                                       vulkan_scene_tree_node *parentNode);

void vulkan_scene_tree_node_add_child(vulkan_scene_tree_node *sceneTreeNode,
                                      vulkan_scene_tree_node *childNode);

void vulkan_scene_tree_node_set_renderer_cache_elements(vulkan_scene_tree_node *sceneTreeNode);
void vulkan_scene_tree_node_accumulate_to_renderer_cache_elements_from_parent(
    vulkan_scene_tree_node *sceneTreeNode);

void vulkan_scene_tree_node_debug_print(vulkan_scene_tree_node *sceneTreeNode);

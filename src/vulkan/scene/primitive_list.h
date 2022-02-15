/* Vulkan scene tree.
 * Contains scene nodes created from scene graph.
 * Used to build and synchronize scene render data.
 */
#pragma once

#include "graph.h"

// HIRO split adding node and adding edge
// HIRO vulkan_scene_primitive struct with vulkan_data_primitive and render cache.
// HIRO tree adds/removes primitives
// HIRO primitive list separates into batches (sort via material and mesh)

typedef struct vulkan_scene_primitive_list {
  vulkan_scene_graph *graph; /// vulkan_scene_graph pointer.

  vulkan_scene_node *root;  /// Root node.
  vulkan_scene_node *nodes; /// Doubly-linked list of all nodes of scene tree.
  UT_array *dirtyNodes;     /// vulkan_scene_node* array of dirty scene tree nodes.
} vulkan_scene_primitive_list;

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_graph *sceneGraph);
void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *sceneTree);

void vulkan_scene_primitive_list_set_dirty(vulkan_scene_primitive_list *sceneTree,
                                           vulkan_scene_node *sceneNode);

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *sceneTree);

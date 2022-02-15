/* Vulkan scene primitive list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Builds primitive batches.
 */
#pragma once

#include "graph.h"

// HIRO primitive list separates into batches (sort via material and mesh)

typedef struct vulkan_scene_primitive_list {
  vulkan_scene_tree *sceneTree; /// vulkan_scene_tree pointer.

  UT_array *primitives; /// vulkan_scene_node* array of primitive scene nodes.
} vulkan_scene_primitive_list;

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree);
void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList);

void vulkan_scene_primitive_list_add_primitive(vulkan_scene_primitive_list *primitiveList,
                                               vulkan_scene_node *primitiveSceneTreeNode);

void vulkan_scene_primitive_list_remove_primitive(vulkan_scene_primitive_list *primitiveList,
                                                  vulkan_scene_node *primitiveSceneTreeNode);

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList);

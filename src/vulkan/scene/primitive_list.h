/* Vulkan scene primitive list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Builds draw call batches.
 */
#pragma once

#include "graph.h"

// HIRO primitive list separates into batches (sort via material and mesh)
// HIRO Should even use flags? Different batch if either change in cache of change in mesh.

/// Each flag imposes constraint on a batch.
typedef enum vulkan_scene_batch_flags {
  vulkan_scene_batch_primitive_flag = (1 << 0), /// Same primitive.
  vulkan_scene_batch_material_flag = (1 << 1),  /// Same material.
} vulkan_scene_batch_flags;

/// Draw call batch.
typedef struct vulkan_scene_batch {
  vulkan_scene_batch_flags flags;
  UT_array *primitiveNodes; /// vulkan_scene_node * list
  // HIRO vulkan draw call data

  struct vulkan_scene_batch *prev, *next; /// List of all batches in primitive list.
} vulkan_scene_batch;

vulkan_scene_batch *vulkan_scene_batch_create(vulkan_scene_batch_flags flags);
// HIRO add primitive node, validate with flags
void vulkan_scene_batch_destroy(vulkan_scene_batch *batch);

typedef struct vulkan_scene_primitive_list {
  vulkan_scene_tree *sceneTree; /// vulkan_scene_tree pointer.

  UT_array
      *primitiveNodes; /// vulkan_scene_node* array of primitive scene nodes. Sorted into batches.
  vulkan_scene_batch *batches; //? List of batches created from primitive scene nodes.
} vulkan_scene_primitive_list;

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree);
void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList);

void vulkan_scene_primitive_list_add_primitive(vulkan_scene_primitive_list *primitiveList,
                                               vulkan_scene_node *primitiveSceneTreeNode);

void vulkan_scene_primitive_list_remove_primitive(vulkan_scene_primitive_list *primitiveList,
                                                  vulkan_scene_node *primitiveSceneTreeNode);
// HIRO sort primitives, split into batches
void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList);

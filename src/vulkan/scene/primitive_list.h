/* Vulkan scene primitive list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Builds draw call batches.
 */
#pragma once

#include "graph.h"

// HIRO primitive list separates into batches (sort via material and mesh)
// HIRO Should even use flags? Different batch if either change in cache of change in mesh.

/// Each flag imposes constraint on a batch, which results in smaller batches and more draw calls.
typedef enum vulkan_scene_batch_flags {
  vulkan_scene_batch_none_flag = 0,                     /// No constraints, one big batch.
  vulkan_scene_batch_vertex_attributes_flag = (1 << 0), /// Same vertex attributes.
  vulkan_scene_batch_material_flag = (1 << 1),          /// Same material.
  vulkan_scene_batch_vertex_all_flag =
      vulkan_scene_batch_vertex_attributes_flag | vulkan_scene_batch_material_flag,
} vulkan_scene_batch_flags;

/// Draw call batch.
typedef struct vulkan_scene_batch {
  vulkan_scene_batch_flags flags;
  vulkan_scene_cache *caches; /// List of vulkan_scene_cache
  // HIRO vulkan draw call data

  struct vulkan_scene_batch *prev, *next; /// List of all batches in primitive list.
} vulkan_scene_batch;

vulkan_scene_batch *vulkan_scene_batch_create(vulkan_scene_batch_flags flags);
void vulkan_scene_batch_destroy(vulkan_scene_batch *batch);

bool vulkan_scene_batch_matches_cache(vulkan_scene_batch *batch, vulkan_scene_cache *cache);
void vulkan_scene_batch_add_cache(vulkan_scene_batch *batch, vulkan_scene_cache *cache);
// HIRO sort via node cache
void vulkan_scene_batch_debug_print(vulkan_scene_batch *batch);

typedef struct vulkan_scene_primitive_list {
  vulkan_scene_tree *sceneTree; /// vulkan_scene_tree pointer.

  UT_array *caches;            /// vulkan_scene_cache* array of primitive scene node caches.
  vulkan_scene_batch *batches; /// List of batches created from primitive scene nodes.
} vulkan_scene_primitive_list;

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree);
void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList);

void vulkan_scene_primitive_list_add_cache(vulkan_scene_primitive_list *primitiveList,
                                           vulkan_scene_cache *cache);

void vulkan_scene_primitive_list_remove_cache(vulkan_scene_primitive_list *primitiveList,
                                              vulkan_scene_cache *cache);

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList);

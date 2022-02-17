/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "../core/core.h"
#include "scene/cache_list.h"

typedef enum vulkan_batch_policy {
  vulkan_batch_policy_none = 0,
  /// Caches in batch have to have same materials.
  vulkan_batch_policy_matching_materials = (1 << 0),
  /// Caches in batch have to have same vertex attributes.
  vulkan_batch_policy_matching_vertex_attributes = (1 << 1),
} vulkan_batch_policy;

/// Draw call batch.
/// Depends on order of caches in cache list.
typedef struct vulkan_batch {
  vulkan_batch_policy policy;
  vulkan_data_primitive *primitive; /// Geometry and material used to draw this batch.
  uint32_t first;                   /// Index to first cache in cache list.
  uint32_t count;                   /// Number of caches in cache list drawn by this batch.

  // HIRO generate VkDrawIndexedIndirectCommand command;

  struct vulkan_batch *prev, *next; /// Linked list.
} vulkan_batch;

vulkan_batch *vulkan_batch_create(vulkan_batch_policy policy, vulkan_scene_cache *firstCache);
void vulkan_batch_destroy(vulkan_batch *batch);

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_scene_cache *cache);

void vulkan_batch_debug_print(vulkan_batch *batch);

/// Creates batches from scene graph.
typedef struct vulkan_batches {
  vulkan_scene_cache_list *cacheList; /// Pointer.

  vulkan_batch *batches; /// List of batches created from scene graph.
} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_scene_graph *sceneGraph);
void vulkan_batches_destroy(vulkan_batches *batches);

/// Destroy old batches.
void vulkan_batches_reset(vulkan_batches *batches);

/// Create and destroy batches using scene graph.
void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_policy policy);

// HIRO: Generate Vulkan draw commands.

void vulkan_batches_debug_print(vulkan_batches *batches);

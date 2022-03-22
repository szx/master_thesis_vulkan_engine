/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "render_cache_list.h"

typedef enum vulkan_batch_policy {
  vulkan_batch_policy_none = 0,
  /// Caches in batch have to have same materials.
  vulkan_batch_policy_matching_materials = (1 << 0),
  /// Caches in batch have to have same vertex attributes.
  vulkan_batch_policy_matching_vertex_attributes = (1 << 1),
} vulkan_batch_policy;

/// Draw call batch.
/// Corresponds to one indirect draw command with instancing.
/// Encapsulates contiguous sequence of caches in cache list.
/// Depends on order of caches in cache list.
typedef struct vulkan_batch {
  vulkan_batch_policy policy;
  vulkan_render_cache *firstCache; /// Geometry and material used to draw this cache.

  /// Indirect draw command.
  /// indexCount - number of vertices to draw, calculated from first cache's primitive
  /// instanceCount - number of instances to draw, equals count
  /// firstIndex - base vertex within the index buffer, equals 0
  /// vertexOffset - vertex buffer offset, equals to first cache's vertex stream offset
  /// firstInstance - the instance ID of the first instance to draw, equals first
  VkDrawIndexedIndirectCommand drawCommand;

  struct vulkan_batch *prev, *next; /// Linked list.
} vulkan_batch;

vulkan_batch *vulkan_batch_create(vulkan_batch_policy policy, vulkan_render_cache *firstCache);
void vulkan_batch_destroy(vulkan_batch *batch);

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_render_cache *cache);
void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_render_cache *cache,
                            size_t renderCacheListIdx);
void vulkan_batch_update_draw_command(vulkan_batch *batch);
void vulkan_batch_record_draw_command(vulkan_batch *batch, VkCommandBuffer commandBuffer);

void vulkan_batch_debug_print(vulkan_batch *batch);

/// Creates batches from scene graph.
typedef struct vulkan_batches {
  vulkan_render_cache_list *renderCacheList; /// Pointer.

  vulkan_batch *batches; /// List of batches created from scene graph.
  // HIRO merge batches?
} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_render_cache_list *renderCacheList);
void vulkan_batches_destroy(vulkan_batches *batches);

/// Destroy old batches.
void vulkan_batches_reset(vulkan_batches *batches);

/// Create and destroy batches using scene graph.
void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_policy policy);

// HIRO: Generate Vulkan draw commands.

void vulkan_batches_debug_print(vulkan_batches *batches);

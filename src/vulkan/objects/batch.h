/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "buffer.h"
#include "render_cache_list.h"

typedef enum vulkan_batch_instancing_policy {
  /// No instancing - one render cache always results in one draw command with instanceCount == 1.
  vulkan_batch_instancing_policy_no_instancing,
  /// Instancing - render caches with same vertex attributes result in one draw command with
  /// instanceCount > 1.
  vulkan_batch_instancing_policy_matching_vertex_attributes,
} vulkan_batch_instancing_policy;

/// Draw call batch.
/// Corresponds to one indirect draw command with instancing.
/// Encapsulates contiguous sequence of caches in cache list.
/// Depends on order of caches in cache list.
typedef struct vulkan_batch {
  vulkan_batch_instancing_policy policy;
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

vulkan_batch *vulkan_batch_create(vulkan_batch_instancing_policy policy,
                                  vulkan_render_cache *firstCache);
void vulkan_batch_destroy(vulkan_batch *batch);

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_render_cache *cache);
void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_render_cache *cache, size_t instanceId);
void vulkan_batch_update_draw_command(vulkan_batch *batch);

void vulkan_batch_debug_print(vulkan_batch *batch);

/// Buffers used by recorded indirect draw commands.
typedef struct vulkan_batches_data {
  vulkan_buffer *indirectDrawBuffer;
  vulkan_buffer_element indirectDrawBufferElement;
} vulkan_batches_data;

void vulkan_batches_data_init(vulkan_batches_data *batchesData, vulkan_device *vkd);

void vulkan_batches_data_deinit(vulkan_batches_data *batchesData);

void vulkan_batches_data_send_to_device(vulkan_batches_data *batchesData);

/// Creates batches from render cache list.
/// Used to record draw commands.
typedef struct vulkan_batches {
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_device *vkd;                        ///< Pointer.

  /// List of batches created from scene graph.
  vulkan_batch *batches;

} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_render_cache_list *renderCacheList,
                                      vulkan_device *vkd);
void vulkan_batches_destroy(vulkan_batches *batches);

void vulkan_batches_reset(vulkan_batches *batches);

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_instancing_policy policy);

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer,
                                        vulkan_batches_data *batchesData);

void vulkan_batches_debug_print(vulkan_batches *batches);

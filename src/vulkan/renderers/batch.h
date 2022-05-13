/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "../objects/buffer.h"
#include "../renderers/renderer_cache.h"

typedef enum vulkan_draw_call_instancing_policy {
  /// No instancing - one renderer cache primitive element always results in one draw command with
  /// instanceCount == 1.
  vulkan_draw_call_instancing_policy_no_instancing,
  /// Instancing - renderer cache primitive elements with same vertex attributes result in one draw
  /// command with
  /// instanceCount > 1.
  vulkan_draw_call_instancing_policy_matching_vertex_attributes,
} vulkan_draw_call_instancing_policy;

/// Draw call / batch.
/// Corresponds to one indirect draw command with instancing.
/// Maintains list of pointers to renderer cache elements from renderer cache that can be rendered
/// using one draw command (draw call batching).
typedef struct vulkan_draw_call {
  vulkan_draw_call_instancing_policy policy;
  vulkan_renderer_cache_primitive_element
      *firstCache; /// Geometry and material used to draw this cache.

  /// Indirect draw command.
  /// indexCount - number of vertices to draw, calculated from first cache's primitive
  /// instanceCount - number of instances to draw, equals count
  /// firstIndex - base vertex within the index buffer, equals 0
  /// vertexOffset - vertex buffer offset, equals to first cache's vertex stream offset
  /// firstInstance - the instance ID of the first instance to draw, equals first
  VkDrawIndexedIndirectCommand drawCommand;

  struct vulkan_draw_call *prev, *next; /// Linked list.
} vulkan_draw_call;

vulkan_draw_call *vulkan_draw_call_create(vulkan_draw_call_instancing_policy policy,
                                          vulkan_renderer_cache_primitive_element *firstCache);
void vulkan_draw_call_destroy(vulkan_draw_call *batch);

bool vulkan_draw_call_matching_cache(vulkan_draw_call *batch,
                                     vulkan_renderer_cache_primitive_element *cache);
void vulkan_draw_call_add_cache(vulkan_draw_call *batch,
                                vulkan_renderer_cache_primitive_element *cache, size_t instanceId);
void vulkan_draw_call_update_draw_command(vulkan_draw_call *batch);

void vulkan_draw_call_debug_print(vulkan_draw_call *batch);

/// Buffers used by recorded indirect draw commands.
typedef struct vulkan_draw_call_data {
  vulkan_buffer *indirectDrawBuffer;
  vulkan_buffer_element indirectDrawBufferElement;
} vulkan_draw_call_data;

void vulkan_draw_call_data_init(vulkan_draw_call_data *batchesData, vulkan_device *vkd);

void vulkan_draw_call_data_deinit(vulkan_draw_call_data *batchesData);

void vulkan_draw_call_data_send_to_device(vulkan_draw_call_data *batchesData);

/// Creates batches from renderer cache.
/// Used to record draw commands.
typedef struct vulkan_batches {
  // HIRO refactor remove rendererCache from batches
  vulkan_renderer_cache *rendererCache; ///< Pointer.

  vulkan_device *vkd; ///< Pointer.

  /// List of created batches.
  vulkan_draw_call *batches;

} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_renderer_cache *rendererCache, vulkan_device *vkd);
void vulkan_batches_destroy(vulkan_batches *batches);

void vulkan_batches_reset(vulkan_batches *batches);

void vulkan_batches_update(vulkan_batches *batches, vulkan_draw_call_instancing_policy policy);

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer,
                                        vulkan_draw_call_data *batchesData);

void vulkan_batches_debug_print(vulkan_batches *batches);

/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "../objects/buffer.h"
#include "../renderers/renderer_cache_elements.h"

typedef enum vulkan_batch_instancing_policy {
  /// No instancing - one renderer cache primitive element always results in one draw command with
  /// instanceCount == 1.
  vulkan_batch_instancing_policy_no_instancing,
  /// Instancing - renderer cache primitive elements with same vertex attributes result in one draw
  /// command with
  /// instanceCount > 1.
  vulkan_batch_instancing_policy_matching_vertex_attributes,
} vulkan_batch_instancing_policy;

/// Draw call / batch.
/// Corresponds to one indirect draw command with instancing.
/// Maintains list of pointers to renderer cache elements from renderer cache that can be rendered
/// using one draw command (draw call batching).
typedef struct vulkan_batch {

  /// Geometry and material used to draw this cache.
  vulkan_renderer_cache_primitive_element *firstPrimitiveElement;

  /// Indirect draw command.
  /// indexCount - number of vertices to draw, calculated from first primitive element
  /// instanceCount - number of instances to draw, equals count
  /// firstIndex - base vertex within the index buffer, equals 0
  /// vertexOffset - vertex buffer offset, equals to first primitive element's vertex stream offset
  /// firstInstance - the instance ID of the first instance to draw, equals first primitive
  /// element's instanceId
  VkDrawIndexedIndirectCommand drawCommand;

} vulkan_batch;

void vulkan_batch_init(vulkan_batch *batch,
                       vulkan_renderer_cache_primitive_element *firstPrimitiveElement,
                       size_t firstInstanceId);

void vulkan_batch_deinit(vulkan_batch *batch);

bool vulkan_batch_matching_primitive_element(vulkan_batch *batch,
                                             vulkan_renderer_cache_primitive_element *cache,
                                             vulkan_batch_instancing_policy policy);
void vulkan_batch_add_primitive_element(vulkan_batch *batch,
                                        vulkan_renderer_cache_primitive_element *cache,
                                        size_t instanceId);

void vulkan_batch_update_draw_command(vulkan_batch *batch);

void vulkan_batch_record_basic_draw_command(vulkan_batch *batch, VkCommandBuffer commandBuffer);

void vulkan_batch_debug_print(vulkan_batch *batch);

/// Buffers used by recorded indirect draw commands.
typedef struct vulkan_batches_buffer {
  vulkan_buffer *indirectDrawBuffer;
  vulkan_buffer_element indirectDrawBufferElement;
} vulkan_batches_buffer;

void vulkan_batches_buffer_init(vulkan_batches_buffer *batchesData, vulkan_device *vkd);

void vulkan_batches_buffer_deinit(vulkan_batches_buffer *batchesData);

void vulkan_batches_buffer_send_to_device(vulkan_batches_buffer *batchesData);

/// Creates batches from renderer cache.
/// Used to record draw commands.
typedef struct vulkan_batches {
  vulkan_device *vkd; ///< Pointer.

  /// vulkan_renderer_cache_primitive_element* array
  /// Pointers to renderer cache primitive elements to be drawn.
  UT_array *primitiveElements;

  /// vulkan_batch array
  /// Created from primitiveElements.
  /// Used to record draw commands.
  UT_array *array;

  bool _primitiveElementsDirty;
} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_device *vkd);
void vulkan_batches_destroy(vulkan_batches *batches);

void vulkan_batches_add_primitive_element(
    vulkan_batches *batches, vulkan_renderer_cache_primitive_element *primitiveElement);

void vulkan_batches_reset(vulkan_batches *batches);

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_instancing_policy policy);

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer,
                                        vulkan_batches_buffer *batchesData);

void vulkan_batches_debug_print(vulkan_batches *batches);

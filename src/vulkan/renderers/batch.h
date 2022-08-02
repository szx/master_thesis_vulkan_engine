/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "../objects/buffer.h"
#include "../renderers/renderer_cache_elements.h"

typedef enum batch_instancing_policy {
  /// No instancing - one renderer cache primitive element always results in one draw command with
  /// instanceCount == 1.
  batch_instancing_policy_no_instancing,
  /// Instancing - renderer cache primitive elements with same vertex attributes result in one draw
  /// command with
  /// instanceCount > 1.
  batch_instancing_policy_matching_vertex_attributes,
} batch_instancing_policy;

/// Draw call / batch.
/// Corresponds to one indirect draw command with instancing.
/// Maintains list of pointers to renderer cache elements from renderer cache that can be rendered
/// using one draw command (draw call batching).
typedef struct batch {

  /// Geometry and material used to draw this cache.
  renderer_cache_primitive_element *firstPrimitiveElement;

  /// Indirect draw command.
  /// indexCount - number of vertices to draw, calculated from first primitive element
  /// instanceCount - number of instances to draw, equals count
  /// firstIndex - base vertex within the index buffer, equals 0
  /// vertexOffset - vertex buffer offset, equals to first primitive element's vertex stream offset
  /// firstInstance - the instance ID of the first instance to draw, equals first primitive
  /// element's instanceId
  VkDrawIndexedIndirectCommand drawCommand;

} batch;

void batch_init(batch *batch, renderer_cache_primitive_element *firstPrimitiveElement,
                size_t firstInstanceId);

void batch_deinit(batch *batch);

bool batch_matching_primitive_element(batch *batch, renderer_cache_primitive_element *cache,
                                      batch_instancing_policy policy);
void batch_add_primitive_element(batch *batch, renderer_cache_primitive_element *cache,
                                 size_t instanceId);

void batch_update_draw_command(batch *batch);

void batch_record_basic_draw_command(batch *batch, VkCommandBuffer commandBuffer);

void batch_debug_print(batch *batch);

/// Buffers used by recorded indirect draw commands.
typedef struct batches_buffer {
  buffer *indirectDrawBuffer;
  buffer_element indirectDrawBufferElement;
} batches_buffer;

void batches_buffer_init(batches_buffer *batchesData, device *vkd);

void batches_buffer_deinit(batches_buffer *batchesData);

void batches_buffer_send_to_device(batches_buffer *batchesData);

/// Creates batches from renderer cache.
/// Used to record draw commands.
typedef struct batches {
  device *vkd; ///< Pointer.

  /// renderer_cache_primitive_element* array
  /// Pointers to renderer cache primitive elements to be drawn.
  UT_array *primitiveElements;

  /// batch array
  /// Created from primitiveElements.
  /// Used to record draw commands.
  UT_array *array;

  bool _primitiveElementsDirty;
} batches;

batches *batches_create(device *vkd);
void batches_destroy(batches *batches);

void batches_add_primitive_element(batches *batches,
                                   renderer_cache_primitive_element *primitiveElement);

void batches_reset(batches *batches);

void batches_update(batches *batches, batch_instancing_policy policy);

void batches_record_draw_command(batches *batches, VkCommandBuffer commandBuffer,
                                 batches_buffer *batchesData);

void batches_debug_print(batches *batches);

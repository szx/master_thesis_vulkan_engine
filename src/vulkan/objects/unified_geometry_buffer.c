#include "unified_geometry_buffer.h"

vulkan_unified_geometry_buffer *
vulkan_unified_geometry_buffer_create(vulkan_device *vkd,
                                      vulkan_render_cache_list *renderCacheList) {
  vulkan_unified_geometry_buffer *geometryBuffer =
      core_alloc(sizeof(vulkan_unified_geometry_buffer));

  geometryBuffer->renderCacheList = renderCacheList;
  geometryBuffer->vertexStream = vulkan_vertex_stream_create(geometryBuffer->renderCacheList);

  geometryBuffer->indexBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_index);
  geometryBuffer->vertexBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_vertex);

  return geometryBuffer;
}

void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer) {
  vulkan_buffer_destroy(geometryBuffer->indexBuffer);
  vulkan_buffer_destroy(geometryBuffer->vertexBuffer);
  vulkan_vertex_stream_destroy(geometryBuffer->vertexStream);

  core_free(geometryBuffer);
}

void vulkan_unified_geometry_buffer_update(vulkan_unified_geometry_buffer *geometryBuffer) {
  vulkan_vertex_stream_update(geometryBuffer->vertexStream);
}

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer) {
  if (!geometryBuffer->vertexStream->dirty) {
    return;
  }

  if (geometryBuffer->indexBuffer->totalSize == 0) {
    vulkan_buffer_add(geometryBuffer->indexBuffer,
                      utarray_front(geometryBuffer->vertexStream->indexData),
                      utarray_size(geometryBuffer->vertexStream->indexData));
    geometryBuffer->indexBuffer->dirty = true;
  }
  vulkan_buffer_send_to_device(geometryBuffer->indexBuffer);

  if (geometryBuffer->vertexBuffer->totalSize == 0) {
    vulkan_buffer_add(geometryBuffer->vertexBuffer,
                      utarray_front(geometryBuffer->vertexStream->vertexData),
                      utarray_size(geometryBuffer->vertexStream->vertexData));
    geometryBuffer->vertexBuffer->dirty = true;
  }

  // TODO: Allow modification of unified vertex buffer if stream is dirty.
  vulkan_buffer_send_to_device(geometryBuffer->vertexBuffer);

  geometryBuffer->vertexStream->dirty = false;
}

void vulkan_unified_geometry_buffer_record_bind_command(
    vulkan_unified_geometry_buffer *geometryBuffer, VkCommandBuffer commandBuffer) {
  assert(!geometryBuffer->vertexStream->dirty);

  VkBuffer vertexBuffers[] = {geometryBuffer->vertexBuffer->buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  VkBuffer indexBuffer = geometryBuffer->indexBuffer->buffer;
  size_t indexBufferStride = utarray_eltsize(geometryBuffer->vertexStream->indexData);
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                       vulkan_stride_to_index_type(indexBufferStride));
}

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer) {
  log_debug("UNIFIED GEOMETRY BUFFER:\n");
  log_debug("index buffer size=%d\n", geometryBuffer->indexBuffer->totalSize);
  log_debug("vertex buffer size=%d\n", geometryBuffer->vertexBuffer->totalSize);
}

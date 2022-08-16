#include "unified_geometry_buffer.h"

unified_geometry_buffer *unified_geometry_buffer_create(device *vkd) {
  unified_geometry_buffer *geometryBuffer = core_alloc(sizeof(unified_geometry_buffer));

  geometryBuffer->dirtyVertexStream = NULL;

  geometryBuffer->indexBuffer = buffer_create(vkd, buffer_type_geometry_index);
  geometryBuffer->vertexBuffer = buffer_create(vkd, buffer_type_geometry_vertex);

  return geometryBuffer;
}

void unified_geometry_buffer_destroy(unified_geometry_buffer *geometryBuffer) {
  buffer_destroy(geometryBuffer->indexBuffer);
  buffer_destroy(geometryBuffer->vertexBuffer);

  core_free(geometryBuffer);
}

void unified_geometry_buffer_update(unified_geometry_buffer *geometryBuffer,
                                    vertex_stream *vertexStream) {
  vertex_stream_update(vertexStream);
  if (vertexStream->dirty) {
    geometryBuffer->dirtyVertexStream = vertexStream;
  }
}

void unified_geometry_buffer_send_to_device(unified_geometry_buffer *geometryBuffer) {
  if (geometryBuffer->dirtyVertexStream == NULL) {
    return;
  }
  assert(geometryBuffer->dirtyVertexStream->dirty);

  // TODO: Allow modification of unified geometry buffer if stream is dirty.
  if (geometryBuffer->indexBuffer->totalSize == 0) {
    buffer_add(geometryBuffer->indexBuffer,
               utarray_front(geometryBuffer->dirtyVertexStream->indexData),
               utarray_size(geometryBuffer->dirtyVertexStream->indexData));
    geometryBuffer->indexBuffer->dirty = true;
  }
  buffer_send_to_device(geometryBuffer->indexBuffer);

  if (geometryBuffer->vertexBuffer->totalSize == 0) {
    buffer_add(geometryBuffer->vertexBuffer,
               utarray_front(geometryBuffer->dirtyVertexStream->vertexData),
               utarray_size(geometryBuffer->dirtyVertexStream->vertexData));
    geometryBuffer->vertexBuffer->dirty = true;
  }

  buffer_send_to_device(geometryBuffer->vertexBuffer);

  geometryBuffer->dirtyVertexStream->dirty = false;
  geometryBuffer->dirtyVertexStream = NULL;
}

void unified_geometry_buffer_record_bind_command(unified_geometry_buffer *geometryBuffer,
                                                 VkCommandBuffer commandBuffer) {

  VkBuffer vertexBuffers[] = {geometryBuffer->vertexBuffer->buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  VkBuffer indexBuffer = geometryBuffer->indexBuffer->buffer;
  size_t indexBufferStride = sizeof(uint32_t);
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                       device_stride_to_index_type(indexBufferStride));
}

void unified_geometry_buffer_debug_print(unified_geometry_buffer *geometryBuffer) {
  log_debug("UNIFIED GEOMETRY BUFFER:\n");
  log_debug("index buffer size=%d\n", geometryBuffer->indexBuffer->totalSize);
  log_debug("vertex buffer size=%d\n", geometryBuffer->vertexBuffer->totalSize);
}

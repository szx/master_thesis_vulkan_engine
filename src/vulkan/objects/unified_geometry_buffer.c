#include "unified_geometry_buffer.h"

vulkan_unified_geometry_buffer *vulkan_unified_geometry_buffer_create(vulkan_device *vkd) {
  vulkan_unified_geometry_buffer *geometryBuffer =
      core_alloc(sizeof(vulkan_unified_geometry_buffer));

  geometryBuffer->interleavedVertexStream = NULL;

  geometryBuffer->indexBuffer =
      vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_index, NULL, 0);
  geometryBuffer->vertexBuffer =
      vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_vertex, NULL, 0);

  geometryBuffer->dirty = true;
  return geometryBuffer;
}

void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer) {
  if (geometryBuffer->interleavedVertexStream != NULL) {
    vulkan_interleaved_vertex_stream_destroy(geometryBuffer->interleavedVertexStream);
  }
  vulkan_buffer_destroy(geometryBuffer->indexBuffer);
  vulkan_buffer_destroy(geometryBuffer->vertexBuffer);
  core_free(geometryBuffer);
}

void vulkan_unified_geometry_buffer_set_interleaved_vertex_stream(
    vulkan_unified_geometry_buffer *geometryBuffer, vulkan_interleaved_vertex_stream *stream) {
  assert(geometryBuffer->interleavedVertexStream == NULL);
  geometryBuffer->interleavedVertexStream = stream;
}

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer) {
  if (!geometryBuffer->dirty || geometryBuffer->interleavedVertexStream == NULL) {
    return;
  }

  // HIRO refactor into vulkan_buffer_sent_to_device params
  geometryBuffer->indexBuffer->data =
      utarray_front(geometryBuffer->interleavedVertexStream->indexData);
  geometryBuffer->indexBuffer->size =
      utarray_size(geometryBuffer->interleavedVertexStream->indexData);
  geometryBuffer->indexBuffer->dirty = true;
  vulkan_buffer_send_to_device(geometryBuffer->indexBuffer);

  geometryBuffer->vertexBuffer->data =
      utarray_front(geometryBuffer->interleavedVertexStream->vertexData);
  geometryBuffer->vertexBuffer->size =
      utarray_size(geometryBuffer->interleavedVertexStream->vertexData);
  geometryBuffer->vertexBuffer->dirty = true;
  vulkan_buffer_send_to_device(geometryBuffer->vertexBuffer);

  geometryBuffer->dirty = false;
}

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer) {
  log_debug("UNIFIED GEOMETRY BUFFER:\n");
  assert(geometryBuffer->indexBuffer->size > 0);
  log_debug("index buffer size=%d\n", geometryBuffer->indexBuffer->size);
  log_debug("vertex buffer size=%d\n", geometryBuffer->vertexBuffer->size);
}

#include "unified_geometry_buffer.h"

vulkan_unified_geometry_buffer *
vulkan_unified_geometry_buffer_create(vulkan_device *vkd,
                                      vulkan_interleaved_vertex_stream *stream) {
  vulkan_unified_geometry_buffer *geometryBuffer =
      core_alloc(sizeof(vulkan_unified_geometry_buffer));

  geometryBuffer->interleavedVertexStream = stream;

  geometryBuffer->indexBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_index);
  geometryBuffer->vertexBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_geometry_vertex);

  return geometryBuffer;
}

void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer) {
  vulkan_interleaved_vertex_stream_destroy(geometryBuffer->interleavedVertexStream);

  vulkan_buffer_destroy(geometryBuffer->indexBuffer);
  vulkan_buffer_destroy(geometryBuffer->vertexBuffer);

  core_free(geometryBuffer);
}

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer) {
  assert(geometryBuffer->interleavedVertexStream->dirty == false);

  if (geometryBuffer->indexBuffer->totalSize == 0) {
    vulkan_buffer_add(geometryBuffer->indexBuffer,
                      utarray_front(geometryBuffer->interleavedVertexStream->indexData),
                      utarray_size(geometryBuffer->interleavedVertexStream->indexData));
    geometryBuffer->indexBuffer->dirty = true;
  }
  vulkan_buffer_send_to_device(geometryBuffer->indexBuffer);

  if (geometryBuffer->vertexBuffer->totalSize == 0) {
    vulkan_buffer_add(geometryBuffer->vertexBuffer,
                      utarray_front(geometryBuffer->interleavedVertexStream->vertexData),
                      utarray_size(geometryBuffer->interleavedVertexStream->vertexData));
    geometryBuffer->vertexBuffer->dirty = true;
  }

  // TODO: Allow modification of unified vertex buffer if stream is dirty.
  vulkan_buffer_send_to_device(geometryBuffer->vertexBuffer);
}

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer) {
  log_debug("UNIFIED GEOMETRY BUFFER:\n");
  assert(geometryBuffer->indexBuffer->totalSize > 0);
  assert(geometryBuffer->vertexBuffer->totalSize > 0);
  log_debug("index buffer size=%d\n", geometryBuffer->indexBuffer->totalSize);
  log_debug("vertex buffer size=%d\n", geometryBuffer->vertexBuffer->totalSize);
}

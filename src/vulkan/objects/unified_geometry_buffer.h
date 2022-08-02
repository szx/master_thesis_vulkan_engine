/* Vulkan API helper objects. */

#pragma once

#include "../common.h"
#include "buffer.h"
#include "vertex_stream.h"

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct unified_geometry_buffer {
  /* CPU state */
  vertex_stream *dirtyVertexStream; ///< Not NULL if buffers need to be updated.

  /* GPU state */
  buffer *indexBuffer;
  buffer *vertexBuffer;
} unified_geometry_buffer;

unified_geometry_buffer *unified_geometry_buffer_create(device *vkd);
void unified_geometry_buffer_destroy(unified_geometry_buffer *geometryBuffer);

void unified_geometry_buffer_update(unified_geometry_buffer *geometryBuffer,
                                    vertex_stream *vertexStream);
void unified_geometry_buffer_send_to_device(unified_geometry_buffer *geometryBuffer);

void unified_geometry_buffer_record_bind_command(unified_geometry_buffer *geometryBuffer,
                                                 VkCommandBuffer commandBuffer);

void unified_geometry_buffer_debug_print(unified_geometry_buffer *geometryBuffer);

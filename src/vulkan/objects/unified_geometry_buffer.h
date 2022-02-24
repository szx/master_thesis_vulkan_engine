/* Vulkan API helper objects. */

#pragma once

#include "buffer.h"
#include "common.h"
#include "vertex_stream.h"

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_unified_geometry_buffer {
  /* CPU state */
  vulkan_interleaved_vertex_stream *interleavedVertexStream; ///< Pointer.

  /* GPU state */
  vulkan_buffer *indexBuffer;
  vulkan_buffer *vertexBuffer;
} vulkan_unified_geometry_buffer;

vulkan_unified_geometry_buffer *
vulkan_unified_geometry_buffer_create(vulkan_device *vkd, vulkan_interleaved_vertex_stream *stream);
void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer);

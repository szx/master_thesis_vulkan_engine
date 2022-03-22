/* Vulkan API helper objects. */

#pragma once

#include "../common.h"
#include "buffer.h"
#include "vertex_stream.h"

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_unified_geometry_buffer {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_vertex_stream *vertexStream;

  /* GPU state */
  vulkan_buffer *indexBuffer;
  vulkan_buffer *vertexBuffer;
} vulkan_unified_geometry_buffer;

vulkan_unified_geometry_buffer *
vulkan_unified_geometry_buffer_create(vulkan_device *vkd,
                                      vulkan_render_cache_list *renderCacheList);
void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_update(vulkan_unified_geometry_buffer *geometryBuffer);
void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_record_bind_command(
    vulkan_unified_geometry_buffer *geometryBuffer, VkCommandBuffer commandBuffer);

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer);

/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "batch.h"
#include "render_cache_list.h"
#include "shader.h"
#include "sync.h"
#include "unified_geometry_buffer.h"
#include "unified_uniform_buffer.h"

/// Render state,
/// Contains all CPU and GPU state shared by all pipelines.
typedef struct vulkan_render_state {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_data_camera *camera;                ///< Pointer.
  vulkan_batches *batches;
  vulkan_interleaved_vertex_stream *vertexStream;

  /* GPU state */
  vulkan_device *vkd; ///< Pointer.

  vulkan_unified_geometry_buffer *unifiedGeometryBuffer;
  vulkan_unified_uniform_buffer *unifiedUniformBuffer;

  vulkan_sync *sync;

} vulkan_render_state;

vulkan_render_state *vulkan_render_state_create(vulkan_device *vkd,
                                                vulkan_render_cache_list *renderCacheList,
                                                vulkan_data_camera *camera);

void vulkan_render_state_destroy(vulkan_render_state *renderState);

void vulkan_scene_render_state_update(vulkan_render_state *renderState);

void vulkan_render_state_send_to_device(vulkan_render_state *renderState);

void vulkan_render_state_debug_print(vulkan_render_state *renderState);

/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "batch.h"
#include "camera.h"
#include "lights.h"
#include "render_cache_list.h"
#include "skybox.h"
#include "sync.h"
#include "textures.h"
#include "unified_geometry_buffer.h"
#include "unified_uniform_buffer.h"

/// Render state,
/// Contains all CPU and GPU state that is independent of pipelines.
typedef struct vulkan_render_state {
  vulkan_device *vkd;     ///< Pointer.
  vulkan_swap_chain *vks; ///< Pointer.

  /* CPU state */
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  data_config *config;                       ///< Pointer.
  vulkan_vertex_stream *vertexStream;

  /* GPU state */
  vulkan_unified_geometry_buffer *unifiedGeometryBuffer;
  vulkan_textures *textures;
  vulkan_unified_uniform_buffer *unifiedUniformBuffer;
  vulkan_descriptors *descriptors;
  vulkan_sync *sync;

  /* CPU + GPU state */
  vulkan_camera *camera;
  vulkan_lights *lights;
  vulkan_skybox *skybox;
  vulkan_batches *batches;

} vulkan_render_state;

vulkan_render_state *vulkan_render_state_create(vulkan_swap_chain *vks,
                                                vulkan_render_cache_list *renderCacheList,
                                                data_config *config);

void vulkan_render_state_destroy(vulkan_render_state *renderState);

void vulkan_render_state_update(vulkan_render_state *renderState);

void vulkan_render_state_send_to_device(vulkan_render_state *renderState);

void vulkan_render_state_debug_print(vulkan_render_state *renderState);

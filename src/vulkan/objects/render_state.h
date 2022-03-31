/* Vulkan API helper objects. */

#pragma once

#include "../data/object.h"
#include "batch.h"
#include "camera.h"
#include "render_cache_list.h"
#include "sync.h"
#include "textures.h"
#include "unified_geometry_buffer.h"
#include "unified_uniform_buffer.h"

/// Render state,
/// Contains all CPU and GPU state shared by all pipelines.
typedef struct vulkan_render_state {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_camera *camera;
  vulkan_batches *batches;

  /* GPU state */
  vulkan_device *vkd; ///< Pointer.

  vulkan_unified_geometry_buffer *unifiedGeometryBuffer;
  vulkan_unified_uniform_buffer *unifiedUniformBuffer;
  vulkan_textures *textures;

  vulkan_descriptors *descriptors;
  vulkan_sync *sync;

} vulkan_render_state;

vulkan_render_state *vulkan_render_state_create(vulkan_device *vkd,
                                                vulkan_render_cache_list *renderCacheList);

void vulkan_render_state_destroy(vulkan_render_state *renderState);

void vulkan_scene_render_state_update(vulkan_render_state *renderState);

void vulkan_render_state_send_to_device(vulkan_render_state *renderState);

void vulkan_render_state_debug_print(vulkan_render_state *renderState);

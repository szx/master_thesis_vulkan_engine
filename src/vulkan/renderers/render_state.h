/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "../objects/swap_chain.h"
#include "../objects/sync.h"
#include "../objects/textures.h"
#include "../objects/unified_constant_buffer.h"
#include "../objects/unified_geometry_buffer.h"
#include "renderer_cache.h"

/// Render state,
/// Contains all CPU and GPU state that is completely independent of render passes.
typedef struct render_state {
  device *vkd;     ///< Pointer.
  swap_chain *vks; ///< Pointer.

  /* CPU state */
  renderer_cache *rendererCache;                                       ///< Pointer.
  data_config *config;                                                 ///< Pointer.
  unified_constant_buffer_update_func updateUnifiedConstantBufferFunc; ///< Pointer.
  vertex_stream *vertexStream;

  /* GPU state */
  unified_geometry_buffer *unifiedGeometryBuffer;
  textures *textures;
  unified_constant_buffer *unifiedUniformBuffer;
  descriptors *descriptors;
  sync *sync;

} render_state;

render_state *
render_state_create(swap_chain *vks, renderer_cache *rendererCache, data_config *config,
                    unified_constant_buffer_update_func updateUnifiedConstantBufferFunc);

void render_state_destroy(render_state *renderState);

void render_state_update(render_state *renderState, void *updateUnifiedConstantBufferFuncData);
void render_state_send_to_device(render_state *renderState);

void render_state_debug_print(render_state *renderState);

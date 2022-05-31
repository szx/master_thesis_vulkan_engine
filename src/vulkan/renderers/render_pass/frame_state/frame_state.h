/* Pipeline frame state.
 * All objects unique to frame in flight.
 * Render pass state maintains number of frame states equal to FRAMES_IN_FLIGHT.
 */
#pragma once

#include "../../../objects/image.h"
#include "../../batch.h"

#include "offscreen_texture_state.h"

typedef struct vulkan_render_pass_state vulkan_render_pass_state;

/// Manages frame-specific resources (indirect buffer, depth and G-Buffer textures) used by
/// render passess.
typedef struct vulkan_render_pass_frame_state {

  vulkan_render_pass_state *renderPassState; /// Pointer.

  /// Frame in flight index.
  size_t frameInFlight;

  // HIRO Maintain two batches for transparent and opaque objects.
  vulkan_batches_buffer rendererCacheBatchesData;

  /// G-Buffer state.
  vulkan_render_pass_offscreen_texture_state *gBuffer;

  /// Depth buffer image.
  vulkan_image *depthBufferImage;

} vulkan_render_pass_frame_state;

void vulkan_render_pass_frame_state_init(vulkan_render_pass_frame_state *frameState,
                                         vulkan_render_pass_state *renderPassState,
                                         size_t frameInFlight);

void vulkan_render_pass_frame_state_deinit(vulkan_render_pass_frame_state *frameState);

void vulkan_render_pass_frame_state_reinit_with_new_swap_chain(
    vulkan_render_pass_frame_state *frameState);

void vulkan_render_pass_frame_state_update(vulkan_render_pass_frame_state *frameState);

void vulkan_render_pass_frame_state_send_to_device(vulkan_render_pass_frame_state *frameState);

void vulkan_render_pass_frame_state_set_unified_uniform_buffer(
    vulkan_render_pass_frame_state *frameState, vulkan_global_uniform_buffer_element *global);

void vulkan_render_pass_frame_state_debug_print(vulkan_render_pass_frame_state *frameState,
                                                int indent);

// HIRO rename vulkan_textures_texture_element to vulkan_texture
vulkan_textures_texture_element *
vulkan_render_pass_frame_state_get_offscreen_texture(vulkan_render_pass_frame_state *frameState,
                                                     vulkan_image_type type);

/* Pipeline frame state.
 * All objects unique to frame in flight.
 * Render pass state maintains number of frame states equal to FRAMES_IN_FLIGHT.
 */
#pragma once

#include "../../../objects/image.h"
#include "../../batch.h"

#include "offscreen_texture_state.h"

typedef struct render_pass_state render_pass_state;

/// Manages frame-specific resources (indirect buffer, depth and G-Buffer textures) used by
/// render passess.
typedef struct render_pass_frame_state {

  render_pass_state *renderPassState; /// Pointer.

  /// Frame in flight index.
  size_t frameInFlight;

  // HIRO Maintain two batches for transparent and opaque objects.
  batches_buffer rendererCacheBatchesData;

  /// Offscreen texture state.
  render_pass_offscreen_texture_state *offscreenTextures;

} render_pass_frame_state;

void render_pass_frame_state_init(render_pass_frame_state *frameState,
                                  render_pass_state *renderPassState, size_t frameInFlight);

void render_pass_frame_state_deinit(render_pass_frame_state *frameState);

void render_pass_frame_state_reinit_with_new_swap_chain(render_pass_frame_state *frameState);

void render_pass_frame_state_update(render_pass_frame_state *frameState);

void render_pass_frame_state_send_to_device(render_pass_frame_state *frameState);

void render_pass_frame_state_set_unified_uniform_buffer(render_pass_frame_state *frameState,
                                                        global_uniform_buffer_element *global);

void render_pass_frame_state_debug_print(render_pass_frame_state *frameState, int indent);

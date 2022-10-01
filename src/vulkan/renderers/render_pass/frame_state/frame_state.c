#include "frame_state.h"
#include "../render_pass_state.h"

void render_pass_frame_state_init(render_pass_frame_state *frameState,
                                  render_pass_state *renderPassState, size_t frameInFlight) {
  frameState->renderPassState = renderPassState;
  frameState->frameInFlight = frameInFlight;

  batches_buffer_init(&frameState->rendererCacheBatchesData,
                      frameState->renderPassState->renderState->vkd);

  frameState->offscreenTextures =
      render_pass_offscreen_texture_state_create(frameState->renderPassState->renderState);
}

void render_pass_frame_state_deinit(render_pass_frame_state *frameState) {
  render_pass_offscreen_texture_state_destroy(frameState->offscreenTextures);
  batches_buffer_deinit(&frameState->rendererCacheBatchesData);
}

void render_pass_frame_state_reinit_with_new_swap_chain(render_pass_frame_state *frameState) {
  render_pass_offscreen_texture_state_reinit_with_new_swap_chain(frameState->offscreenTextures);
}

void render_pass_frame_state_update(render_pass_frame_state *frameState) {
  log_debug("updating render pass frame state %zu", frameState->frameInFlight);
  render_pass_offscreen_texture_state_update(frameState->offscreenTextures);
}

void render_pass_frame_state_send_to_device(render_pass_frame_state *frameState) {
  batches_buffer_send_to_device(&frameState->rendererCacheBatchesData);
}

void render_pass_frame_state_set_unified_constant_buffer(render_pass_frame_state *frameState,
                                                         global_uniform_buffer_element *global) {

  render_pass_offscreen_texture_state_set_g_buffer_elements(frameState->offscreenTextures,
                                                            &global->offscreenTextures);
}

void render_pass_frame_state_debug_print(render_pass_frame_state *frameState, int indent) {
  log_debug(INDENT_FORMAT_STRING "render pass frame state (frame #zu):", INDENT_FORMAT_ARGS(0),
            frameState->frameInFlight);
  render_pass_offscreen_texture_state_debug_print(frameState->offscreenTextures, indent + 2);
}

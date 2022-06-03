#include "frame_state.h"
#include "../render_pass_state.h"

void vulkan_render_pass_frame_state_init(vulkan_render_pass_frame_state *frameState,
                                         vulkan_render_pass_state *renderPassState,
                                         size_t frameInFlight) {
  frameState->renderPassState = renderPassState;
  frameState->frameInFlight = frameInFlight;

  vulkan_batches_buffer_init(&frameState->rendererCacheBatchesData,
                             frameState->renderPassState->renderState->vkd);

  frameState->offscreenTextures =
      vulkan_render_pass_offscreen_texture_state_create(frameState->renderPassState->renderState);
}

void vulkan_render_pass_frame_state_deinit(vulkan_render_pass_frame_state *frameState) {
  vulkan_render_pass_offscreen_texture_state_destroy(frameState->offscreenTextures);
  vulkan_batches_buffer_deinit(&frameState->rendererCacheBatchesData);
}

void vulkan_render_pass_frame_state_reinit_with_new_swap_chain(
    vulkan_render_pass_frame_state *frameState) {
  vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
      frameState->offscreenTextures);
}

void vulkan_render_pass_frame_state_update(vulkan_render_pass_frame_state *frameState) {
  log_debug("updating render pass frame state %zu", frameState->frameInFlight);
  vulkan_render_pass_offscreen_texture_state_update(frameState->offscreenTextures);
}

void vulkan_render_pass_frame_state_send_to_device(vulkan_render_pass_frame_state *frameState) {
  vulkan_batches_buffer_send_to_device(&frameState->rendererCacheBatchesData);
}

void vulkan_render_pass_frame_state_set_unified_uniform_buffer(
    vulkan_render_pass_frame_state *frameState, vulkan_global_uniform_buffer_element *global) {

  vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(frameState->offscreenTextures,
                                                                   &global->offscreenTextures);
}

void vulkan_render_pass_frame_state_debug_print(vulkan_render_pass_frame_state *frameState,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "render pass frame state (frame #zu):", INDENT_FORMAT_ARGS(0),
            frameState->frameInFlight);
  vulkan_render_pass_offscreen_texture_state_debug_print(frameState->offscreenTextures, indent + 2);
}

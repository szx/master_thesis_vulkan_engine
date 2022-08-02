#include "render_pass_state.h"

render_pass_state *render_pass_state_create(render_state *renderState) {
  render_pass_state *renderPassState = core_alloc(sizeof(render_pass_state));

  render_pass_state_init(renderPassState, renderState);

  return renderPassState;
}

void render_pass_state_destroy(render_pass_state *renderPassState) {
  render_pass_state_deinit(renderPassState);
  core_free(renderPassState);
}

void render_pass_state_init(render_pass_state *renderPassState, render_state *renderState) {
  renderPassState->renderState = renderState;
  render_pass_shared_state_init(&renderPassState->sharedState, renderPassState->renderState);

  utarray_alloc(renderPassState->frameStates, sizeof(render_pass_frame_state));
  utarray_resize(renderPassState->frameStates, FRAMES_IN_FLIGHT);
  size_t frameInFlight = 0;
  utarray_foreach_elem_it (render_pass_frame_state *, frameState, renderPassState->frameStates) {
    render_pass_frame_state_init(frameState, renderPassState, frameInFlight);
    frameInFlight++;
  }
}

void render_pass_state_deinit(render_pass_state *renderPassState) {
  render_pass_shared_state_deinit(&renderPassState->sharedState);

  utarray_foreach_elem_it (render_pass_frame_state *, frameState, renderPassState->frameStates) {
    render_pass_frame_state_deinit(frameState);
  }
  utarray_free(renderPassState->frameStates);
}

void render_pass_state_reinit_with_new_swap_chain(render_pass_state *renderPassState) {
  render_pass_shared_state_reinit_with_new_swap_chain(&renderPassState->sharedState);

  utarray_foreach_elem_it (render_pass_frame_state *, frameState, renderPassState->frameStates) {
    render_pass_frame_state_reinit_with_new_swap_chain(frameState);
  }
}

void render_pass_state_update(render_pass_state *renderPassState) {
  render_pass_shared_state_update(&renderPassState->sharedState);

  render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  render_pass_frame_state_update(frameState);
}

uint32_t render_pass_state_add_offscreen_texture(render_pass_state *renderPassState,
                                                 const char *name, image_type imageType) {
  uint32_t offscreenTextureIdx = MAX_OFFSCREEN_TEXTURE_COUNT;
  utarray_foreach_elem_it (render_pass_frame_state *, frameState, renderPassState->frameStates) {
    uint32_t newOffscreenTextureIdx = render_pass_offscreen_texture_state_add_offscreen_texture(
        frameState->offscreenTextures, name, imageType);
    assert(offscreenTextureIdx == MAX_OFFSCREEN_TEXTURE_COUNT ||
           offscreenTextureIdx == newOffscreenTextureIdx);
    offscreenTextureIdx = newOffscreenTextureIdx;
  }
  return offscreenTextureIdx;
}

void render_pass_state_set_unified_uniform_buffer(render_pass_state *renderPassState,
                                                  global_uniform_buffer_element *global) {
  render_pass_shared_state_set_unified_uniform_buffer(&renderPassState->sharedState, global);

  render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  render_pass_frame_state_set_unified_uniform_buffer(frameState, global);
}

void render_pass_state_send_to_device(render_pass_state *renderPassState) {
  render_pass_shared_state_send_to_device(&renderPassState->sharedState);

  render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  render_pass_frame_state_send_to_device(frameState);
}

void render_pass_state_debug_print(render_pass_state *renderPassState, int indent) {
  log_debug(INDENT_FORMAT_STRING "render pass state:", INDENT_FORMAT_ARGS(0));
  render_pass_shared_state_debug_print(&renderPassState->sharedState, indent + 2);
  utarray_foreach_elem_it (render_pass_frame_state *, frameState, renderPassState->frameStates) {
    render_pass_frame_state_debug_print(frameState, indent + 2);
  }
}

render_pass_frame_state *render_pass_state_get_frame_state(render_pass_state *renderPassState,
                                                           size_t frameInFlight) {
  return utarray_eltptr(renderPassState->frameStates, frameInFlight);
}

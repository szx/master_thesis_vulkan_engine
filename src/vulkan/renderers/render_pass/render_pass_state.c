#include "render_pass_state.h"

vulkan_render_pass_state *vulkan_render_pass_state_create(vulkan_render_state *renderState) {
  vulkan_render_pass_state *renderPassState = core_alloc(sizeof(vulkan_render_pass_state));

  vulkan_render_pass_state_init(renderPassState, renderState);

  return renderPassState;
}

void vulkan_render_pass_state_destroy(vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass_state_deinit(renderPassState);
  core_free(renderPassState);
}

void vulkan_render_pass_state_init(vulkan_render_pass_state *renderPassState,
                                   vulkan_render_state *renderState) {
  renderPassState->renderState = renderState;
  vulkan_render_pass_shared_state_init(&renderPassState->sharedState, renderPassState->renderState);

  utarray_alloc(renderPassState->frameStates, sizeof(vulkan_render_pass_frame_state));
  utarray_resize(renderPassState->frameStates, FRAMES_IN_FLIGHT);
  size_t frameInFlight = 0;
  utarray_foreach_elem_it (vulkan_render_pass_frame_state *, frameState,
                           renderPassState->frameStates) {
    vulkan_render_pass_frame_state_init(frameState, renderPassState, frameInFlight);
    frameInFlight++;
  }
}

void vulkan_render_pass_state_deinit(vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass_shared_state_deinit(&renderPassState->sharedState);

  utarray_foreach_elem_it (vulkan_render_pass_frame_state *, frameState,
                           renderPassState->frameStates) {
    vulkan_render_pass_frame_state_deinit(frameState);
  }
  utarray_free(renderPassState->frameStates);
}

void vulkan_render_pass_state_reinit_with_new_swap_chain(
    vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass_shared_state_reinit_with_new_swap_chain(&renderPassState->sharedState);

  utarray_foreach_elem_it (vulkan_render_pass_frame_state *, frameState,
                           renderPassState->frameStates) {
    vulkan_render_pass_frame_state_reinit_with_new_swap_chain(frameState);
  }
}

void vulkan_render_pass_state_update(vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass_shared_state_update(&renderPassState->sharedState);

  vulkan_render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  vulkan_render_pass_frame_state_update(frameState);
}

void vulkan_render_pass_state_set_unified_uniform_buffer(
    vulkan_render_pass_state *renderPassState, vulkan_global_uniform_buffer_element *global) {
  vulkan_render_pass_shared_state_set_unified_uniform_buffer(&renderPassState->sharedState, global);

  vulkan_render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  vulkan_render_pass_frame_state_set_unified_uniform_buffer(frameState, global);
}

void vulkan_render_pass_state_send_to_device(vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass_shared_state_send_to_device(&renderPassState->sharedState);

  vulkan_render_pass_frame_state *frameState = utarray_eltptr(
      renderPassState->frameStates, renderPassState->renderState->sync->currentFrameInFlight);
  vulkan_render_pass_frame_state_send_to_device(frameState);
}

void vulkan_render_pass_state_debug_print(vulkan_render_pass_state *renderPassState, int indent) {
  log_debug(INDENT_FORMAT_STRING "render pass state:", INDENT_FORMAT_ARGS(0));
  vulkan_render_pass_shared_state_debug_print(&renderPassState->sharedState, indent + 2);
  utarray_foreach_elem_it (vulkan_render_pass_frame_state *, frameState,
                           renderPassState->frameStates) {
    vulkan_render_pass_frame_state_debug_print(frameState, indent + 2);
  }
}

vulkan_render_pass_frame_state *
vulkan_render_pass_state_get_frame_state(vulkan_render_pass_state *renderPassState,
                                         size_t frameInFlight) {
  return utarray_eltptr(renderPassState->frameStates, frameInFlight);
}

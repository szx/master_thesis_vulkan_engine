#include "pipeline_state.h"

vulkan_pipeline_state *vulkan_pipeline_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_state *pipelineState = core_alloc(sizeof(vulkan_pipeline_state));

  vulkan_pipeline_state_init(pipelineState, renderState);

  return pipelineState;
}

void vulkan_pipeline_state_destroy(vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline_state_deinit(pipelineState);
  core_free(pipelineState);
}

void vulkan_pipeline_state_init(vulkan_pipeline_state *pipelineState,
                                vulkan_render_state *renderState) {
  pipelineState->renderState = renderState;
  vulkan_pipeline_shared_state_init(&pipelineState->sharedState, pipelineState->renderState);

  utarray_alloc(pipelineState->frameStates, sizeof(vulkan_pipeline_frame_state));
  utarray_resize(pipelineState->frameStates, FRAMES_IN_FLIGHT);
  size_t frameInFlight = 0;
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipelineState->frameStates) {
    vulkan_pipeline_frame_state_init(frameState, pipelineState, frameInFlight);
    frameInFlight++;
  }
}

void vulkan_pipeline_state_deinit(vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline_shared_state_deinit(&pipelineState->sharedState);

  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipelineState->frameStates) {
    vulkan_pipeline_frame_state_deinit(frameState);
  }
  utarray_free(pipelineState->frameStates);
}

void vulkan_pipeline_state_reinit_with_new_swap_chain(vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline_shared_state_reinit_with_new_swap_chain(&pipelineState->sharedState);

  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipelineState->frameStates) {
    vulkan_pipeline_frame_state_reinit_with_new_swap_chain(frameState);
  }
}

void vulkan_pipeline_state_update(vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline_shared_state_update(&pipelineState->sharedState);

  vulkan_pipeline_frame_state *frameState = utarray_eltptr(
      pipelineState->frameStates, pipelineState->renderState->sync->currentFrameInFlight);
  vulkan_pipeline_frame_state_update(frameState);
}

void vulkan_pipeline_state_set_unified_uniform_buffer(
    vulkan_pipeline_state *pipelineState, vulkan_global_uniform_buffer_element *global) {
  vulkan_pipeline_shared_state_set_unified_uniform_buffer(&pipelineState->sharedState, global);

  vulkan_pipeline_frame_state *frameState = utarray_eltptr(
      pipelineState->frameStates, pipelineState->renderState->sync->currentFrameInFlight);
  vulkan_pipeline_frame_state_set_unified_uniform_buffer(frameState, global);
}

void vulkan_pipeline_state_send_to_device(vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline_shared_state_send_to_device(&pipelineState->sharedState);

  vulkan_pipeline_frame_state *frameState = utarray_eltptr(
      pipelineState->frameStates, pipelineState->renderState->sync->currentFrameInFlight);
  vulkan_pipeline_frame_state_send_to_device(frameState);
}

void vulkan_pipeline_state_debug_print(vulkan_pipeline_state *pipelineState, int indent) {
  log_debug(INDENT_FORMAT_STRING "pipeline state:", INDENT_FORMAT_ARGS(0));
  vulkan_pipeline_shared_state_debug_print(&pipelineState->sharedState, indent + 2);
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipelineState->frameStates) {
    vulkan_pipeline_frame_state_debug_print(frameState, indent + 2);
  }
}

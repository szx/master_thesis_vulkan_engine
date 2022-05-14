#include "frame_state.h"
#include "pipeline_state.h"

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline_state *pipelineState, size_t frameInFlight) {
  frameState->pipelineState = pipelineState;
  frameState->frameInFlight = frameInFlight;
  vulkan_batches_buffer_init(&frameState->rendererCacheBatchesData,
                             frameState->pipelineState->renderState->vkd);
}

void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState) {
  vulkan_batches_buffer_deinit(&frameState->rendererCacheBatchesData);
}

void vulkan_pipeline_frame_state_reinit_with_new_swap_chain(
    vulkan_pipeline_frame_state *frameState) {
  // No-op.
}

void vulkan_pipeline_frame_state_update(vulkan_pipeline_frame_state *frameState) {
  // No-op.
  log_debug("updating pipeline frame state %zu", frameState->frameInFlight);
}

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState) {
  vulkan_batches_buffer_send_to_device(&frameState->rendererCacheBatchesData);
}

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent) {
  log_debug(INDENT_FORMAT_STRING "pipeline frame state (frame #zu)", INDENT_FORMAT_ARGS(0),
            frameState->frameInFlight);
}

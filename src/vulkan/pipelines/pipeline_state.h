/* Pipelines state.
 * All objects used by pipelines.
 */
#pragma once

#include "../renderers/render_state.h"
#include "frame_state.h"
#include "shared_state.h"

/// Contains all resources used by pipelines.
typedef struct vulkan_pipeline_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_pipeline_shared_state sharedState;

  UT_array *frameStates; ///< vulkan_pipeline_frame_state list

} vulkan_pipeline_state;

vulkan_pipeline_state *vulkan_pipeline_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_state_destroy(vulkan_pipeline_state *pipelineState);

void vulkan_pipeline_state_init(vulkan_pipeline_state *pipelineState,
                                vulkan_render_state *renderState);
void vulkan_pipeline_state_deinit(vulkan_pipeline_state *pipelineState);
void vulkan_pipeline_state_reinit_with_new_swap_chain(vulkan_pipeline_state *pipelineState);

void vulkan_pipeline_state_update(vulkan_pipeline_state *pipelineState);

void vulkan_pipeline_state_send_to_device(vulkan_pipeline_state *pipelineState);

void vulkan_pipeline_state_debug_print(vulkan_pipeline_state *pipelineState, int indent);

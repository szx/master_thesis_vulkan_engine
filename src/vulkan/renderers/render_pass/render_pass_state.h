/* Pipelines state.
 * All objects used by renderPasss.
 */
#pragma once

#include "../render_state.h"
#include "frame_state/frame_state.h"
#include "shared_state/shared_state.h"

/// Contains all resources used by renderPasss.
typedef struct vulkan_render_pass_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_render_pass_shared_state sharedState;

  UT_array *frameStates; ///< vulkan_render_pass_frame_state list

} vulkan_render_pass_state;

vulkan_render_pass_state *vulkan_render_pass_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_state_destroy(vulkan_render_pass_state *renderPassState);

void vulkan_render_pass_state_init(vulkan_render_pass_state *renderPassState,
                                   vulkan_render_state *renderState);
void vulkan_render_pass_state_deinit(vulkan_render_pass_state *renderPassState);
void vulkan_render_pass_state_reinit_with_new_swap_chain(vulkan_render_pass_state *renderPassState);

void vulkan_render_pass_state_update(vulkan_render_pass_state *renderPassState);

uint32_t vulkan_render_pass_state_add_offscreen_texture(vulkan_render_pass_state *renderPassState,
                                                        const char *name,
                                                        vulkan_image_type imageType);

void vulkan_render_pass_state_set_unified_uniform_buffer(
    vulkan_render_pass_state *renderPassState, vulkan_global_uniform_buffer_element *global);

void vulkan_render_pass_state_send_to_device(vulkan_render_pass_state *renderPassState);

void vulkan_render_pass_state_debug_print(vulkan_render_pass_state *renderPassState, int indent);

vulkan_render_pass_frame_state *
vulkan_render_pass_state_get_frame_state(vulkan_render_pass_state *renderPassState,
                                         size_t frameInFlight);

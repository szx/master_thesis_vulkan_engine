/* Pipelines state.
 * All objects used by renderPasss.
 */
#pragma once

#include "../render_state.h"
#include "frame_state/frame_state.h"
#include "shared_state/shared_state.h"

/// Contains all resources used by renderPasss.
typedef struct render_pass_state {
  render_state *renderState; ///< Pointer.

  render_pass_shared_state sharedState;

  UT_array *frameStates; ///< render_pass_frame_state list

} render_pass_state;

render_pass_state *render_pass_state_create(render_state *renderState);
void render_pass_state_destroy(render_pass_state *renderPassState);

void render_pass_state_init(render_pass_state *renderPassState, render_state *renderState);
void render_pass_state_deinit(render_pass_state *renderPassState);
void render_pass_state_reinit_with_new_swap_chain(render_pass_state *renderPassState);

void render_pass_state_update(render_pass_state *renderPassState);

uint32_t render_pass_state_add_offscreen_texture(render_pass_state *renderPassState,
                                                 const char *name, image_type imageType);

void render_pass_state_set_unified_uniform_buffer(render_pass_state *renderPassState,
                                                  global_uniform_buffer_element *global);

void render_pass_state_send_to_device(render_pass_state *renderPassState);

void render_pass_state_debug_print(render_pass_state *renderPassState, int indent);

render_pass_frame_state *render_pass_state_get_frame_state(render_pass_state *renderPassState,
                                                           size_t frameInFlight);

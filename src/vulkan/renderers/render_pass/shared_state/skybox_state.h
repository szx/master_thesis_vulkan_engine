/* Vulkan skybox.
 */
#pragma once

#include "../../render_state.h"

typedef struct render_pass_skybox_state {
  render_state *renderState; ///< Pointer.

  float ambientIntensity;

} render_pass_skybox_state;

render_pass_skybox_state *render_pass_skybox_state_create(render_state *renderState);
void render_pass_skybox_state_destroy(render_pass_skybox_state *skybox);
void render_pass_skybox_state_reinit_with_new_swap_chain(render_pass_skybox_state *skybox);
void render_pass_skybox_state_update(render_pass_skybox_state *skybox);

void render_pass_skybox_state_set_skybox_elements(render_pass_skybox_state *skybox,
                                                  skybox_helper_element *skyboxElement);

void render_pass_skybox_state_debug_print(render_pass_skybox_state *skybox, int indent);

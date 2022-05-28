/* Vulkan skybox.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_skybox_state {
  vulkan_render_state *renderState; ///< Pointer.

  float ambientIntensity;

} vulkan_render_pass_skybox_state;

vulkan_render_pass_skybox_state *
vulkan_render_pass_skybox_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_skybox_state_destroy(vulkan_render_pass_skybox_state *skybox);
void vulkan_render_pass_skybox_state_reinit_with_new_swap_chain(
    vulkan_render_pass_skybox_state *skybox);
void vulkan_render_pass_skybox_state_update(vulkan_render_pass_skybox_state *skybox);

void vulkan_render_pass_skybox_state_set_skybox_elements(
    vulkan_render_pass_skybox_state *skybox, vulkan_skybox_helper_element *skyboxElement);

void vulkan_render_pass_skybox_state_debug_print(vulkan_render_pass_skybox_state *skybox,
                                                 int indent);

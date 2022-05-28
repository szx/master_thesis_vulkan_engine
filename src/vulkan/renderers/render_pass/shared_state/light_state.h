/* Vulkan lights.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_light_state {
  vulkan_render_state *renderState; ///< Pointer.

} vulkan_render_pass_light_state;

vulkan_render_pass_light_state *
vulkan_render_pass_light_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_light_state_destroy(vulkan_render_pass_light_state *lights);
void vulkan_render_pass_light_state_reinit_with_new_swap_chain(
    vulkan_render_pass_light_state *light);
void vulkan_render_pass_light_state_update(vulkan_render_pass_light_state *light);

vulkan_renderer_cache_direct_light_element *
vulkan_render_pass_light_state_select(vulkan_render_pass_light_state *lights,
                                      vulkan_direct_light_type type, size_t lightIdx);

void vulkan_render_pass_light_state_set_direct_light_elements(
    vulkan_render_pass_light_state *lights, uint32_t *directionalLightCount,
    vulkan_directional_light_helper_element *directionalLightElements, uint32_t *pointLightCount,
    vulkan_point_light_helper_element *pointLightElements, uint32_t *spotLightCount,
    vulkan_spot_light_helper_element *spotLightElements);

void vulkan_render_pass_light_state_debug_print(vulkan_render_pass_light_state *lights, int indent);

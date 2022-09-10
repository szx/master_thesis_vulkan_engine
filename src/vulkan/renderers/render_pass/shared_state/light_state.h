/* Vulkan lights.
 */
#pragma once

#include "../../render_state.h"

typedef struct render_pass_light_state {
  render_state *renderState; ///< Pointer.

} render_pass_light_state;

render_pass_light_state *render_pass_light_state_create(render_state *renderState);
void render_pass_light_state_destroy(render_pass_light_state *lights);
void render_pass_light_state_reinit_with_new_swap_chain(render_pass_light_state *light);
void render_pass_light_state_update(render_pass_light_state *light);

renderer_cache_direct_light_element *render_pass_light_state_select(render_pass_light_state *lights,
                                                                    direct_light_type type,
                                                                    size_t lightIdx);

void render_pass_light_state_set_direct_light_elements(
    render_pass_light_state *lights, uint32_t *directionalLightCount,
    directional_light_helper_element *directionalLightElements, uint32_t *pointLightCount,
    point_light_helper_element *pointLightElements);

void render_pass_light_state_debug_print(render_pass_light_state *lights, int indent);

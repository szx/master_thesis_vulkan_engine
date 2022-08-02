/* Vulkan materials.
 */
#pragma once

#include "../../render_state.h"

typedef struct render_pass_material_state {
  render_state *renderState; ///< Pointer.

} render_pass_material_state;

render_pass_material_state *render_pass_material_state_create(render_state *renderState);
void render_pass_material_state_destroy(render_pass_material_state *materials);
void render_pass_material_state_reinit_with_new_swap_chain(render_pass_material_state *material);
void render_pass_material_state_update(render_pass_material_state *material);

void render_pass_material_state_set_material_elements(render_pass_material_state *materials,
                                                      uint32_t *materialCount,
                                                      material_helper_element *materialElements);

void render_pass_material_state_debug_print(render_pass_material_state *materials, int indent);

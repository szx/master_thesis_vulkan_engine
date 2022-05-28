/* Vulkan materials.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_material_state {
  vulkan_render_state *renderState; ///< Pointer.

} vulkan_render_pass_material_state;

vulkan_render_pass_material_state *
vulkan_render_pass_material_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_material_state_destroy(vulkan_render_pass_material_state *materials);
void vulkan_render_pass_material_state_reinit_with_new_swap_chain(
    vulkan_render_pass_material_state *material);
void vulkan_render_pass_material_state_update(vulkan_render_pass_material_state *material);

void vulkan_render_pass_material_state_set_material_elements(
    vulkan_render_pass_material_state *materials, uint32_t *materialCount,
    vulkan_material_helper_element *materialElements);

void vulkan_render_pass_material_state_debug_print(vulkan_render_pass_material_state *materials,
                                                   int indent);

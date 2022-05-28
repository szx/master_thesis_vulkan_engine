/* Vulkan font.
 * Used by debug text renderPass.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_font_state {
  vulkan_render_state *renderState; ///< Pointer.

  bool debugTextEnabled;
  UT_string *text;

} vulkan_render_pass_font_state;

vulkan_render_pass_font_state *
vulkan_render_pass_font_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_font_state_destroy(vulkan_render_pass_font_state *font);
void vulkan_render_pass_font_state_reinit_with_new_swap_chain(vulkan_render_pass_font_state *font);
void vulkan_render_pass_font_state_update(vulkan_render_pass_font_state *font);

void vulkan_render_pass_font_state_set_font_elements(vulkan_render_pass_font_state *font,
                                                     vulkan_font_helper_element *fontElement);

void vulkan_render_pass_font_state_debug_print(vulkan_render_pass_font_state *font, int indent);

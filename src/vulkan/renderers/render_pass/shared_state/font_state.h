/* Vulkan font.
 * Used by debug text renderPass.
 */
#pragma once

#include "../../render_state.h"

typedef struct render_pass_font_state {
  render_state *renderState; ///< Pointer.

  bool debugTextEnabled;
  UT_string *text;

} render_pass_font_state;

render_pass_font_state *render_pass_font_state_create(render_state *renderState);
void render_pass_font_state_destroy(render_pass_font_state *font);
void render_pass_font_state_reinit_with_new_swap_chain(render_pass_font_state *font);
void render_pass_font_state_update(render_pass_font_state *font);

void render_pass_font_state_set_font_elements(render_pass_font_state *font,
                                              font_helper_element *fontElement);

void render_pass_font_state_debug_print(render_pass_font_state *font, int indent);

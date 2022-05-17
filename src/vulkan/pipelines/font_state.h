/* Vulkan font.
 * Used by debug text pipeline.
 */
#pragma once

#include "../common.h"
#include "../renderers/render_state.h"

typedef struct vulkan_pipeline_font_state {
  vulkan_render_state *renderState; ///< Pointer.

  float ambientIntensity;

} vulkan_pipeline_font_state;

vulkan_pipeline_font_state *vulkan_pipeline_font_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_font_state_destroy(vulkan_pipeline_font_state *font);
void vulkan_pipeline_font_state_reinit_with_new_swap_chain(vulkan_pipeline_font_state *font);
void vulkan_pipeline_font_state_update(vulkan_pipeline_font_state *font);

void vulkan_pipeline_font_state_set_font_elements(vulkan_pipeline_font_state *font,
                                                  vulkan_font_helper_element *fontElement);

void vulkan_pipeline_font_state_debug_print(vulkan_pipeline_font_state *font, int indent);

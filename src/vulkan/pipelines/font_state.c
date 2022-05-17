#include "font_state.h"

vulkan_pipeline_font_state *vulkan_pipeline_font_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_font_state *font = core_alloc(sizeof(vulkan_pipeline_font_state));

  font->renderState = renderState;

  // HIRO remove ambientIntensity from font
  font->ambientIntensity = 1.0f;

  return font;
}

void vulkan_pipeline_font_state_destroy(vulkan_pipeline_font_state *font) { core_free(font); }

void vulkan_pipeline_font_state_reinit_with_new_swap_chain(vulkan_pipeline_font_state *font) {
  // No-op.
}

void vulkan_pipeline_font_state_update(vulkan_pipeline_font_state *font) {
  // No-op.
  // HIRO Update font helper struct with text (after conversion)
}

void vulkan_pipeline_font_state_set_font_elements(vulkan_pipeline_font_state *font,
                                                  vulkan_font_helper_element *fontElement) {
  fontElement->fontTextureId = font->renderState->rendererCache->fontElement->fontMaterialElement
                                   ->baseColorTextureElement->textureIdx;
  fontElement->characterSize = font->renderState->rendererCache->fontElement->font->characterSize;
  fontElement->fontTextureSize =
      font->renderState->rendererCache->fontElement->font->fontTexture->image->width;
  // HIRO Update font helper struct with text (after conversion)
}

void vulkan_pipeline_font_state_debug_print(vulkan_pipeline_font_state *font, int indent) {
  log_debug(INDENT_FORMAT_STRING "font:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "ambientIntensity: %f", INDENT_FORMAT_ARGS(2),
            font->ambientIntensity);
}

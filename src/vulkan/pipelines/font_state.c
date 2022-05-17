#include "font_state.h"

vulkan_pipeline_font_state *vulkan_pipeline_font_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_font_state *font = core_alloc(sizeof(vulkan_pipeline_font_state));

  font->renderState = renderState;

  utstring_new(font->text);

  return font;
}

void vulkan_pipeline_font_state_destroy(vulkan_pipeline_font_state *font) {
  utstring_free(font->text);
  core_free(font);
}

void vulkan_pipeline_font_state_reinit_with_new_swap_chain(vulkan_pipeline_font_state *font) {
  // No-op.
}

void vulkan_pipeline_font_state_update(vulkan_pipeline_font_state *font) {
  // No-op.
}

void vulkan_pipeline_font_state_set_font_elements(vulkan_pipeline_font_state *font,
                                                  vulkan_font_helper_element *fontElement) {
  fontElement->fontTextureId = font->renderState->rendererCache->fontElement->fontMaterialElement
                                   ->baseColorTextureElement->textureIdx;
  fontElement->characterSize = font->renderState->rendererCache->fontElement->font->characterSize;
  fontElement->fontTextureSize =
      font->renderState->rendererCache->fontElement->font->fontTexture->image->width;

  // HIRO pack text
  fontElement->textLength = utstring_len(font->text);
  UT_string *characters = font->renderState->rendererCache->fontElement->font->characters;
  size_t widthInCharacters =
      font->renderState->vks->swapChainExtent.width / fontElement->characterSize;
  size_t textIdx = 0;
  for (size_t i = 0; i < utstring_len(font->text); i++) {
    const char character = utstring_body(font->text)[i];
    if (character == '\n') {
      size_t remaining = widthInCharacters - (textIdx % widthInCharacters);
      for (size_t j = 0; j < remaining; j++) {
        fontElement->text[textIdx + j] = 0;
      }
      textIdx += remaining;
    } else {
      char characterIdx = 0;
      for (size_t j = 0; j < utstring_len(characters); j++) {
        if (character == utstring_body(characters)[j]) {
          characterIdx = j;
          break;
        }
      }
      fontElement->text[textIdx] = characterIdx;
      textIdx++;
    }
  }
}

void vulkan_pipeline_font_state_debug_print(vulkan_pipeline_font_state *font, int indent) {
  log_debug(INDENT_FORMAT_STRING "font:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "text: %s", INDENT_FORMAT_ARGS(2), utstring_body(font->text));
}

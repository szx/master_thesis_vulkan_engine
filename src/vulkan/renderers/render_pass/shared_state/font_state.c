#include "font_state.h"

render_pass_font_state *render_pass_font_state_create(render_state *renderState) {
  render_pass_font_state *font = core_alloc(sizeof(render_pass_font_state));

  font->renderState = renderState;

  font->debugTextEnabled = true;
  utstring_new(font->text);

  return font;
}

void render_pass_font_state_destroy(render_pass_font_state *font) {
  utstring_free(font->text);
  core_free(font);
}

void render_pass_font_state_reinit_with_new_swap_chain(render_pass_font_state *font) {
  // No-op.
}

void render_pass_font_state_update(render_pass_font_state *font) {
  // No-op.
}

void render_pass_font_state_set_font_elements(render_pass_font_state *font,
                                              font_helper_element *fontElement) {
  fontElement->fontTextureId =
      font->renderState->rendererCache->fontElement->fontTextureElement->textureIdx;
  fontElement->characterSize = font->renderState->rendererCache->fontElement->font->characterSize;
  fontElement->fontTextureSize =
      font->renderState->rendererCache->fontElement->font->fontTexture->image->width;

  if (font->debugTextEnabled) {
    core_memset(fontElement->text, 0, MAX_TEXT_CHARACTER_COUNT);
    UT_string *characters = font->renderState->rendererCache->fontElement->font->characters;
    size_t widthInCharacters =
        font->renderState->vks->swapChainExtent.width / fontElement->characterSize;
    size_t textIdx = 0;
    size_t textOffset = 0;
    for (size_t i = 0; i < utstring_len(font->text); i++) {
      const char character = utstring_body(font->text)[i];
      if (character == '\n') {
        size_t remaining = (widthInCharacters - ((textIdx * 4) % widthInCharacters)) / 4;
        for (size_t j = 1; j < remaining; j++) {
          fontElement->text[textIdx + j] = 0;
        }
        textIdx += remaining;
        textOffset = 0;

      } else {
        char characterIdx = 0;
        for (size_t j = 0; j < utstring_len(characters); j++) {
          if (character == utstring_body(characters)[j]) {
            characterIdx = j;
            break;
          }
        }

        // Pack 4 characters in one uint.
        uint32_t mask = 0xFFu << (textOffset * 8);
        fontElement->text[textIdx] &= ~mask;
        fontElement->text[textIdx] |= characterIdx << (textOffset * 8);

        textOffset = (textOffset + 1) % 4;
        if (textOffset == 0) {
          textIdx++;
        }
      }
    }
    fontElement->textLength = textIdx;
  } else {
    fontElement->textLength = 0;
  }
}

void render_pass_font_state_debug_print(render_pass_font_state *font, int indent) {
  log_debug(INDENT_FORMAT_STRING "font:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "text: %s", INDENT_FORMAT_ARGS(2), utstring_body(font->text));
}

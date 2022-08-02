#include "offscreen_texture_state.h"

render_pass_offscreen_texture_state *
render_pass_offscreen_texture_state_create(render_state *renderState) {
  render_pass_offscreen_texture_state *gBuffer =
      core_alloc(sizeof(render_pass_offscreen_texture_state));

  gBuffer->renderState = renderState;

  gBuffer->offscreenTextureElementCount = 0;

  return gBuffer;
}

void render_pass_offscreen_texture_state_destroy(render_pass_offscreen_texture_state *gBuffer) {
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    asset_image_deinit(gBuffer->offscreenTextureAssets[i]->image);
    core_free(gBuffer->offscreenTextureAssets[i]->image);
    asset_sampler_deinit(gBuffer->offscreenTextureAssets[i]->sampler);
    core_free(gBuffer->offscreenTextureAssets[i]->sampler);
    asset_texture_deinit(gBuffer->offscreenTextureAssets[i]);
    core_free(gBuffer->offscreenTextureAssets[i]);
  }
  core_free(gBuffer);
}

void render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    render_pass_offscreen_texture_state *gBuffer) {
  uint32_t width = gBuffer->renderState->vks->swapChainExtent.width;
  uint32_t height = gBuffer->renderState->vks->swapChainExtent.height;
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    gBuffer->offscreenTextureElements[i]->image->resident = false;
    gBuffer->offscreenTextureElements[i]->image->width = width;
    gBuffer->offscreenTextureElements[i]->image->height = height;
  }
}

void render_pass_offscreen_texture_state_update(render_pass_offscreen_texture_state *gBuffer) {
  // No-op.
}

uint32_t render_pass_offscreen_texture_state_add_offscreen_texture(
    render_pass_offscreen_texture_state *gBuffer, const char *name, image_type imageType) {
  uint32_t idx = gBuffer->offscreenTextureElementCount++;
  assert(idx < MAX_OFFSCREEN_TEXTURE_COUNT);

  gBuffer->offscreenTextureNames[idx] = name;

  gBuffer->offscreenTextureAssets[idx] =
      asset_texture_create_from_params(imageType, gBuffer->renderState->vks->swapChainExtent.width,
                                       gBuffer->renderState->vks->swapChainExtent.height, 1, 4);

  gBuffer->offscreenTextureElements[idx] =
      textures_add_texture(gBuffer->renderState->textures, gBuffer->offscreenTextureAssets[idx],
                           gBuffer->offscreenTextureNames[idx]);

  return idx;
}

void render_pass_offscreen_texture_state_set_g_buffer_elements(
    render_pass_offscreen_texture_state *gBuffer,
    offscreen_texture_helper_element *gBufferHelperElement) {
  assert(gBuffer->offscreenTextureElementCount > 0);
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    gBufferHelperElement->textureId[i] = gBuffer->offscreenTextureElements[i]->textureIdx;
  }
}

void render_pass_offscreen_texture_state_debug_print(render_pass_offscreen_texture_state *gBuffer,
                                                     int indent) {
  log_debug(INDENT_FORMAT_STRING "offscreen texture state:", INDENT_FORMAT_ARGS(0));
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    log_debug(INDENT_FORMAT_STRING "offscreen texture #%zu:", INDENT_FORMAT_ARGS(2), i);
    asset_texture_debug_print(gBuffer->offscreenTextureElements[i]->texture, indent + 4);
  }
}

textures_texture_element *render_pass_offscreen_texture_state_get_offscreen_texture(
    render_pass_offscreen_texture_state *gBuffer, const char *name) {
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    if (strcmp(name, gBuffer->offscreenTextureNames[i]) == 0) {
      return gBuffer->offscreenTextureElements[i];
    }
  }
  UNREACHABLE;
}

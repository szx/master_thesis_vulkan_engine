#include "offscreen_texture_state.h"

vulkan_render_pass_offscreen_texture_state *
vulkan_render_pass_offscreen_texture_state_create(vulkan_render_state *renderState) {
  vulkan_render_pass_offscreen_texture_state *gBuffer =
      core_alloc(sizeof(vulkan_render_pass_offscreen_texture_state));

  gBuffer->renderState = renderState;

  gBuffer->offscreenTextureElementCount = 0;

  return gBuffer;
}

void vulkan_render_pass_offscreen_texture_state_destroy(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    vulkan_asset_image_deinit(gBuffer->offscreenTextureAssets[i]->image);
    core_free(gBuffer->offscreenTextureAssets[i]->image);
    vulkan_asset_sampler_deinit(gBuffer->offscreenTextureAssets[i]->sampler);
    core_free(gBuffer->offscreenTextureAssets[i]->sampler);
    vulkan_asset_texture_deinit(gBuffer->offscreenTextureAssets[i]);
    core_free(gBuffer->offscreenTextureAssets[i]);
  }
  core_free(gBuffer);
}

void vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  uint32_t width = gBuffer->renderState->vks->swapChainExtent.width;
  uint32_t height = gBuffer->renderState->vks->swapChainExtent.height;
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    gBuffer->offscreenTextureElements[i]->image->resident = false;
    gBuffer->offscreenTextureElements[i]->image->width = width;
    gBuffer->offscreenTextureElements[i]->image->height = height;
  }
}

void vulkan_render_pass_offscreen_texture_state_update(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  // No-op.
}

uint32_t vulkan_render_pass_offscreen_texture_state_add_offscreen_texture(
    vulkan_render_pass_offscreen_texture_state *gBuffer, const char *name,
    vulkan_image_type imageType) {
  uint32_t idx = gBuffer->offscreenTextureElementCount++;
  assert(idx < MAX_OFFSCREEN_TEXTURE_COUNT);

  gBuffer->offscreenTextureNames[idx] = name;

  gBuffer->offscreenTextureAssets[idx] = vulkan_asset_texture_create_from_params(
      imageType, gBuffer->renderState->vks->swapChainExtent.width,
      gBuffer->renderState->vks->swapChainExtent.height, 1, 4);

  gBuffer->offscreenTextureElements[idx] = vulkan_textures_add_texture(
      gBuffer->renderState->textures, gBuffer->offscreenTextureAssets[idx],
      gBuffer->offscreenTextureNames[idx]);

  return idx;
}

void vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(
    vulkan_render_pass_offscreen_texture_state *gBuffer,
    vulkan_offscreen_texture_helper_element *gBufferHelperElement) {
  assert(gBuffer->offscreenTextureElementCount > 0);
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    gBufferHelperElement->textureId[i] = gBuffer->offscreenTextureElements[i]->textureIdx;
  }
}

void vulkan_render_pass_offscreen_texture_state_debug_print(
    vulkan_render_pass_offscreen_texture_state *gBuffer, int indent) {
  log_debug(INDENT_FORMAT_STRING "offscreen texture state:", INDENT_FORMAT_ARGS(0));
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    log_debug(INDENT_FORMAT_STRING "offscreen texture #%zu:", INDENT_FORMAT_ARGS(2), i);
    vulkan_asset_texture_debug_print(gBuffer->offscreenTextureElements[i]->texture, indent + 4);
  }
}

vulkan_textures_texture_element *vulkan_render_pass_offscreen_texture_state_get_offscreen_texture(
    vulkan_render_pass_offscreen_texture_state *gBuffer, const char *name) {
  for (size_t i = 0; i < gBuffer->offscreenTextureElementCount; i++) {
    if (strcmp(name, gBuffer->offscreenTextureNames[i]) == 0) {
      return gBuffer->offscreenTextureElements[i];
    }
  }
  UNREACHABLE;
}

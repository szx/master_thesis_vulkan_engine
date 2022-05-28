#include "offscreen_texture_state.h"

vulkan_render_pass_offscreen_texture_state *
vulkan_render_pass_offscreen_texture_state_create(vulkan_render_state *renderState) {
  vulkan_render_pass_offscreen_texture_state *gBuffer =
      core_alloc(sizeof(vulkan_render_pass_offscreen_texture_state));

  gBuffer->renderState = renderState;

  vulkan_asset_texture *gBuffer0Texture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_0,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->gBuffer0TextureElement =
      vulkan_textures_add_texture(renderState->textures, gBuffer0Texture);

  vulkan_asset_texture *gBuffer1Texture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_1,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->gBuffer1TextureElement =
      vulkan_textures_add_texture(renderState->textures, gBuffer1Texture);

  vulkan_asset_texture *gBuffer2Texture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_2,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->gBuffer2TextureElement =
      vulkan_textures_add_texture(renderState->textures, gBuffer2Texture);

  return gBuffer;
}

void vulkan_render_pass_offscreen_texture_state_destroy(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  core_free(gBuffer);
}

void vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  uint32_t width = gBuffer->renderState->vks->swapChainExtent.width;
  uint32_t height = gBuffer->renderState->vks->swapChainExtent.height;

  gBuffer->gBuffer0TextureElement->image->resident = false;
  gBuffer->gBuffer0TextureElement->image->width = width;
  gBuffer->gBuffer0TextureElement->image->height = height;

  gBuffer->gBuffer1TextureElement->image->resident = false;
  gBuffer->gBuffer1TextureElement->image->width = width;
  gBuffer->gBuffer1TextureElement->image->height = height;

  gBuffer->gBuffer2TextureElement->image->resident = false;
  gBuffer->gBuffer2TextureElement->image->width = width;
  gBuffer->gBuffer2TextureElement->image->height = height;
}

void vulkan_render_pass_offscreen_texture_state_update(
    vulkan_render_pass_offscreen_texture_state *gBuffer) {
  // No-op.
}

void vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(
    vulkan_render_pass_offscreen_texture_state *gBuffer,
    vulkan_g_buffer_helper_element *gBufferHelperElement) {
  gBufferHelperElement->gBuffer0TextureId = gBuffer->gBuffer0TextureElement->textureIdx;
  gBufferHelperElement->gBuffer1TextureId = gBuffer->gBuffer1TextureElement->textureIdx;
  gBufferHelperElement->gBuffer2TextureId = gBuffer->gBuffer2TextureElement->textureIdx;
}

void vulkan_render_pass_offscreen_texture_state_debug_print(
    vulkan_render_pass_offscreen_texture_state *gBuffer, int indent) {
  log_debug(INDENT_FORMAT_STRING "gBuffer:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "gBuffer0Texture:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->gBuffer0TextureElement->texture, indent + 4);
  log_debug(INDENT_FORMAT_STRING "gBuffer1TextureId:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->gBuffer1TextureElement->texture, indent + 4);
  log_debug(INDENT_FORMAT_STRING "gBuffer2TextureId:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->gBuffer2TextureElement->texture, indent + 4);
}

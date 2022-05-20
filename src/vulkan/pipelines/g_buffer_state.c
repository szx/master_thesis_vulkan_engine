#include "g_buffer_state.h"

vulkan_pipeline_g_buffer_state *
vulkan_pipeline_g_buffer_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_g_buffer_state *gBuffer = core_alloc(sizeof(vulkan_pipeline_g_buffer_state));

  gBuffer->renderState = renderState;

  vulkan_asset_texture *worldPositionTexture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_0,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->worldPositionTextureElement =
      vulkan_textures_add_texture(renderState->textures, worldPositionTexture);

  vulkan_asset_texture *diffuseTexture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_1,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->baseColorTextureElement =
      vulkan_textures_add_texture(renderState->textures, diffuseTexture);

  vulkan_asset_texture *normalTexture = vulkan_asset_texture_create_from_params(
      renderState->rendererCache->sceneData, vulkan_image_type_g_buffer_2,
      renderState->vks->swapChainExtent.width, renderState->vks->swapChainExtent.height, 1, 4);
  gBuffer->normalTextureElement = vulkan_textures_add_texture(renderState->textures, normalTexture);

  return gBuffer;
}

void vulkan_pipeline_g_buffer_state_destroy(vulkan_pipeline_g_buffer_state *gBuffer) {
  core_free(gBuffer);
}

void vulkan_pipeline_g_buffer_state_reinit_with_new_swap_chain(
    vulkan_pipeline_g_buffer_state *gBuffer) {
  uint32_t width = gBuffer->renderState->vks->swapChainExtent.width;
  uint32_t height = gBuffer->renderState->vks->swapChainExtent.height;

  gBuffer->worldPositionTextureElement->image->resident = false;
  gBuffer->worldPositionTextureElement->image->width = width;
  gBuffer->worldPositionTextureElement->image->height = height;

  gBuffer->baseColorTextureElement->image->resident = false;
  gBuffer->baseColorTextureElement->image->width = width;
  gBuffer->baseColorTextureElement->image->height = height;

  gBuffer->normalTextureElement->image->resident = false;
  gBuffer->normalTextureElement->image->width = width;
  gBuffer->normalTextureElement->image->height = height;
}

void vulkan_pipeline_g_buffer_state_update(vulkan_pipeline_g_buffer_state *gBuffer) {
  // No-op.
}

void vulkan_pipeline_g_buffer_state_set_g_buffer_elements(
    vulkan_pipeline_g_buffer_state *gBuffer, vulkan_g_buffer_helper_element *gBufferHelperElement) {
  gBufferHelperElement->worldPositionTextureId = gBuffer->worldPositionTextureElement->textureIdx;
  gBufferHelperElement->diffuseTextureId = gBuffer->baseColorTextureElement->textureIdx;
  gBufferHelperElement->normalTextureId = gBuffer->normalTextureElement->textureIdx;
}

void vulkan_pipeline_g_buffer_state_debug_print(vulkan_pipeline_g_buffer_state *gBuffer,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "gBuffer:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "worldPositionTexture:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->worldPositionTextureElement->texture, indent + 4);
  log_debug(INDENT_FORMAT_STRING "baseColorTextureElement:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->baseColorTextureElement->texture, indent + 4);
  log_debug(INDENT_FORMAT_STRING "normalTextureElement:", INDENT_FORMAT_ARGS(2));
  vulkan_asset_texture_debug_print(gBuffer->normalTextureElement->texture, indent + 4);
}

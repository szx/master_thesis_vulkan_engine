#include "frame_state.h"
#include "../render_pass_state.h"

void vulkan_render_pass_frame_state_init(vulkan_render_pass_frame_state *frameState,
                                         vulkan_render_pass_state *renderPassState,
                                         size_t frameInFlight) {
  frameState->renderPassState = renderPassState;
  frameState->frameInFlight = frameInFlight;

  vulkan_batches_buffer_init(&frameState->rendererCacheBatchesData,
                             frameState->renderPassState->renderState->vkd);

  frameState->gBuffer =
      vulkan_render_pass_offscreen_texture_state_create(frameState->renderPassState->renderState);

  frameState->depthBufferImage = vulkan_image_create(
      frameState->renderPassState->renderState->vkd, vulkan_image_type_depth_buffer,
      frameState->renderPassState->renderState->vks->swapChainExtent.width,
      frameState->renderPassState->renderState->vks->swapChainExtent.height, 1);
}

void vulkan_render_pass_frame_state_deinit(vulkan_render_pass_frame_state *frameState) {
  vulkan_image_destroy(frameState->depthBufferImage);
  vulkan_render_pass_offscreen_texture_state_destroy(frameState->gBuffer);
  vulkan_batches_buffer_deinit(&frameState->rendererCacheBatchesData);
}

void vulkan_render_pass_frame_state_reinit_with_new_swap_chain(
    vulkan_render_pass_frame_state *frameState) {
  vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(frameState->gBuffer);

  vulkan_image_destroy(frameState->depthBufferImage);
  frameState->depthBufferImage = vulkan_image_create(
      frameState->renderPassState->renderState->vkd, vulkan_image_type_depth_buffer,
      frameState->renderPassState->renderState->vks->swapChainExtent.width,
      frameState->renderPassState->renderState->vks->swapChainExtent.height, 1);
}

void vulkan_render_pass_frame_state_update(vulkan_render_pass_frame_state *frameState) {
  log_debug("updating render pass frame state %zu", frameState->frameInFlight);
  vulkan_render_pass_offscreen_texture_state_update(frameState->gBuffer);
}

void vulkan_render_pass_frame_state_send_to_device(vulkan_render_pass_frame_state *frameState) {
  vulkan_batches_buffer_send_to_device(&frameState->rendererCacheBatchesData);
  vulkan_image_send_to_device(frameState->depthBufferImage);
}

void vulkan_render_pass_frame_state_set_unified_uniform_buffer(
    vulkan_render_pass_frame_state *frameState, vulkan_global_uniform_buffer_element *global) {

  vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(frameState->gBuffer,
                                                                   &global->gBuffer);
}

void vulkan_render_pass_frame_state_debug_print(vulkan_render_pass_frame_state *frameState,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "render pass frame state (frame #zu):", INDENT_FORMAT_ARGS(0),
            frameState->frameInFlight);
  vulkan_render_pass_offscreen_texture_state_debug_print(frameState->gBuffer, indent + 2);
  vulkan_image_debug_print(frameState->depthBufferImage, indent + 2);
}

vulkan_image *vulkan_render_pass_frame_state_get_offscreen_framebuffer_attachment_image(
    vulkan_render_pass_frame_state *frameState, vulkan_offscreen_texture_type type) {
  if (type == vulkan_offscreen_texture_type_g_buffer_0) {
    return frameState->gBuffer->gBuffer0TextureElement->image;
  }
  if (type == vulkan_offscreen_texture_type_g_buffer_1) {
    return frameState->gBuffer->gBuffer1TextureElement->image;
  }
  if (type == vulkan_offscreen_texture_type_g_buffer_2) {
    return frameState->gBuffer->gBuffer2TextureElement->image;
  }
  UNREACHABLE;
}

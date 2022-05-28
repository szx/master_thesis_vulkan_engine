#include "render_pass_impl.h"
#include "render_pass.h"

/* forward */

vulkan_render_pass_info
vulkan_render_pass_impl_forward_get_renderPass_info(vulkan_render_pass *renderPass) {
  return (vulkan_render_pass_info){.useOnscreenColorAttachment = true,
                                   .onscreenClearValue =
                                       (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                   .offscreenTextureCount = 0,
                                   .useDepthAttachment = true,
                                   .depthAttachmentWriteEnable = true,
                                   .depthAttachmentTestEnable = true,
                                   .depthAttachmentTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                                   .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                   .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                                   .depthClearValue = (VkClearDepthStencilValue){0.0f, 0},
                                   .colorBlendingType = vulkan_color_blending_type_none};
}

void vulkan_render_pass_impl_forward_record_commands(vulkan_render_pass *renderPass,
                                                     vulkan_render_pass_frame_state *frameState,
                                                     VkCommandBuffer commandBuffer) {

  vulkan_batches_record_draw_command(renderPass->renderPassState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);
}

/* deferred geometry */

vulkan_render_pass_info
vulkan_render_pass_impl_deferred_geometry_get_renderPass_info(vulkan_render_pass *renderPass) {
  return (vulkan_render_pass_info){
      .useOnscreenColorAttachment = false,
      .onscreenClearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},

      .offscreenTextureCount = 3,
      .offscreenTextureInfos =
          {
              {.type = vulkan_offscreen_texture_type_g_buffer_0,
               .usage = vulkan_offscreen_texture_usage_framebuffer_color_attachment},
              {.type = vulkan_offscreen_texture_type_g_buffer_1,
               .usage = vulkan_offscreen_texture_usage_framebuffer_color_attachment},
              {.type = vulkan_offscreen_texture_type_g_buffer_2,
               .usage = vulkan_offscreen_texture_usage_framebuffer_color_attachment},
          },
      .offscreenClearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},

      .useDepthAttachment = true,
      .depthAttachmentWriteEnable = true,
      .depthAttachmentTestEnable = true,
      .depthAttachmentTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
      .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
      .depthClearValue = {0.0f, 0},

      .colorBlendingType = vulkan_color_blending_type_none};
}

void vulkan_render_pass_impl_deferred_geometry_record_commands(
    vulkan_render_pass *renderPass, vulkan_render_pass_frame_state *frameState,
    VkCommandBuffer commandBuffer) {

  vulkan_batches_record_draw_command(renderPass->renderPassState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);
}

/* deferred lighting */

vulkan_render_pass_info
vulkan_render_pass_impl_deferred_lighting_get_renderPass_info(vulkan_render_pass *renderPass) {
  return (vulkan_render_pass_info){
      .useOnscreenColorAttachment = true,
      .offscreenTextureCount = 3,
      .offscreenTextureInfos =
          {
              {.type = vulkan_offscreen_texture_type_g_buffer_0,
               .usage = vulkan_offscreen_texture_usage_fragment_shader_read},
              {.type = vulkan_offscreen_texture_type_g_buffer_1,
               .usage = vulkan_offscreen_texture_usage_fragment_shader_read},
              {.type = vulkan_offscreen_texture_type_g_buffer_2,
               .usage = vulkan_offscreen_texture_usage_fragment_shader_read},
          },
      // Use depth buffer to reject fragments with depth == 0 (no geometry rendered)
      .useDepthAttachment = true,
      .depthAttachmentWriteEnable = false,
      .depthAttachmentTestEnable = true,
      .depthAttachmentTestOp = VK_COMPARE_OP_LESS,
      .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
      .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
  };
}

void vulkan_render_pass_impl_deferred_lighting_record_commands(
    vulkan_render_pass *renderPass, vulkan_render_pass_frame_state *frameState,
    VkCommandBuffer commandBuffer) {
  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      renderPass->renderPassState->renderState->rendererCache
          ->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

/* skybox */

vulkan_render_pass_info
vulkan_render_pass_impl_skybox_get_renderPass_info(vulkan_render_pass *renderPass) {
  return (vulkan_render_pass_info){
      .useOnscreenColorAttachment = true,
      .onscreenClearValue = (VkClearColorValue){{0.5f, 0.5f, 0.5f, 1.0f}},
      .offscreenTextureCount = 0,
      // Use depth buffer to reject fragments with depth != 0
      .useDepthAttachment = true,
      .depthAttachmentWriteEnable = false,
      .depthAttachmentTestEnable = true,
      .depthAttachmentTestOp = VK_COMPARE_OP_EQUAL,
      .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
      .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_NONE_EXT,
  };
}

void vulkan_render_pass_impl_skybox_record_commands(vulkan_render_pass *renderPass,
                                                    vulkan_render_pass_frame_state *frameState,
                                                    VkCommandBuffer commandBuffer) {
  // Draw cube.
  vulkan_renderer_cache_primitive_element *basicBoxPrimitiveElement =
      renderPass->renderPassState->renderState->rendererCache->basicBoxPrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicBoxPrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

/* debug text */

vulkan_render_pass_info
vulkan_render_pass_impl_debug_text_get_renderPass_info(vulkan_render_pass *renderPass) {
  return (vulkan_render_pass_info){.useOnscreenColorAttachment = true,
                                   .colorBlendingType = vulkan_color_blending_type_alpha};
}

void vulkan_render_pass_impl_debug_text_record_commands(vulkan_render_pass *renderPass,
                                                        vulkan_render_pass_frame_state *frameState,
                                                        VkCommandBuffer commandBuffer) {
  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      renderPass->renderPassState->renderState->rendererCache
          ->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

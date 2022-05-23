#include "pipeline_impl.h"
#include "pipeline.h"

/* forward */

vulkan_pipeline_info vulkan_pipeline_impl_forward_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
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

void vulkan_pipeline_impl_forward_record_render_pass(vulkan_pipeline *pipeline,
                                                     vulkan_pipeline_frame_state *frameState,
                                                     VkCommandBuffer commandBuffer) {

  vulkan_batches_record_draw_command(pipeline->pipelineState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);
}

/* deferred geometry */

vulkan_pipeline_info
vulkan_pipeline_impl_deferred_geometry_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){
      .useOnscreenColorAttachment = false,
      .onscreenClearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},

      .offscreenTextureCount = 3,
      .offscreenTextureInfos =
          {
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_0,
               .usage = vulkan_pipeline_offscreen_texture_usage_framebuffer_color_attachment},
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_1,
               .usage = vulkan_pipeline_offscreen_texture_usage_framebuffer_color_attachment},
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_2,
               .usage = vulkan_pipeline_offscreen_texture_usage_framebuffer_color_attachment},
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

void vulkan_pipeline_impl_deferred_geometry_record_render_pass(
    vulkan_pipeline *pipeline, vulkan_pipeline_frame_state *frameState,
    VkCommandBuffer commandBuffer) {

  vulkan_batches_record_draw_command(pipeline->pipelineState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);
}

/* deferred lighting */

vulkan_pipeline_info
vulkan_pipeline_impl_deferred_lighting_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){
      .useOnscreenColorAttachment = true,
      .offscreenTextureCount = 3,
      .offscreenTextureInfos =
          {
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_0,
               .usage = vulkan_pipeline_offscreen_texture_usage_fragment_shader_read},
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_1,
               .usage = vulkan_pipeline_offscreen_texture_usage_fragment_shader_read},
              {.type = vulkan_pipeline_offscreen_texture_type_g_buffer_2,
               .usage = vulkan_pipeline_offscreen_texture_usage_fragment_shader_read},
          },
      // Use depth buffer to reject fragments with depth == 0 (no geometry rendered)
      .useDepthAttachment = true,
      .depthAttachmentWriteEnable = false,
      .depthAttachmentTestEnable = true,
      .depthAttachmentTestOp = VK_COMPARE_OP_LESS,
      .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
      .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_NONE_EXT,
  };
}

void vulkan_pipeline_impl_deferred_lighting_record_render_pass(
    vulkan_pipeline *pipeline, vulkan_pipeline_frame_state *frameState,
    VkCommandBuffer commandBuffer) {
  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

/* skybox */

vulkan_pipeline_info vulkan_pipeline_impl_skybox_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){
      .useOnscreenColorAttachment = true,
      .onscreenClearValue = (VkClearColorValue){{0.5f, 0.5f, 0.5f, 1.0f}},
      .offscreenTextureCount = 0,
  };
}

void vulkan_pipeline_impl_skybox_record_render_pass(vulkan_pipeline *pipeline,
                                                    vulkan_pipeline_frame_state *frameState,
                                                    VkCommandBuffer commandBuffer) {
  // Draw cube.
  vulkan_renderer_cache_primitive_element *basicBoxPrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicBoxPrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicBoxPrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

/* debug text */

vulkan_pipeline_info vulkan_pipeline_impl_debug_text_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .colorBlendingType = vulkan_color_blending_type_alpha};
}

void vulkan_pipeline_impl_debug_text_record_render_pass(vulkan_pipeline *pipeline,
                                                        vulkan_pipeline_frame_state *frameState,
                                                        VkCommandBuffer commandBuffer) {
  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

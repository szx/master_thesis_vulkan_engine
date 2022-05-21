#include "pipeline_impl.h"
#include "pipeline.h"

/* forward */

vulkan_pipeline_info vulkan_pipeline_impl_forward_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                .offscreenColorAttachmentCount = 0,
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
                                                     VkCommandBuffer commandBuffer,
                                                     size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->framebuffers, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->pipelineState->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  // HIRO pass render pass and framebuffer as argument, allocate if NULL
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = *framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

  vulkan_batches_record_draw_command(pipeline->pipelineState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);

  vkCmdEndRenderPass(commandBuffer);
}

/* deferred geometry */

vulkan_pipeline_info
vulkan_pipeline_impl_deferred_geometry_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = false,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                .offscreenColorAttachmentCount = 3,
                                .offscreenColorAttachmentTypes =
                                    {
                                        vulkan_pipeline_offscreen_attachment_type_g_buffer_0,
                                        vulkan_pipeline_offscreen_attachment_type_g_buffer_1,
                                        vulkan_pipeline_offscreen_attachment_type_g_buffer_2,
                                    },
                                .useDepthAttachment = true,
                                .depthAttachmentWriteEnable = true,
                                .depthAttachmentTestEnable = true,
                                .depthAttachmentTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                                .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                                .depthClearValue = (VkClearDepthStencilValue){0.0f, 0},
                                .colorBlendingType = vulkan_color_blending_type_none};
}

void vulkan_pipeline_impl_deferred_geometry_record_render_pass(vulkan_pipeline *pipeline,
                                                               VkCommandBuffer commandBuffer,
                                                               size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->framebuffers, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->pipelineState->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = *framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

  vulkan_batches_record_draw_command(pipeline->pipelineState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);

  vkCmdEndRenderPass(commandBuffer);
}

/* deferred lighting */

vulkan_pipeline_info
vulkan_pipeline_impl_deferred_lighting_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){
      .useOnscreenColorAttachment = true,
      // Use depth buffer to reject fragments with depth == 0 (no geometry rendered)
      .useDepthAttachment = true,
      .depthAttachmentWriteEnable = false,
      .depthAttachmentTestEnable = true,
      .depthAttachmentTestOp = VK_COMPARE_OP_LESS,
      .depthAttachmentLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
      .depthAttachmentStoreOp = VK_ATTACHMENT_STORE_OP_NONE_EXT,
  };
}

void vulkan_pipeline_impl_deferred_lighting_record_render_pass(vulkan_pipeline *pipeline,
                                                               VkCommandBuffer commandBuffer,
                                                               size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->framebuffers, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;

  /* record new render pass into command buffer */
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = *framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);

  vkCmdEndRenderPass(commandBuffer);
}

/* skybox */

vulkan_pipeline_info vulkan_pipeline_impl_skybox_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){
      .useOnscreenColorAttachment = true,
      .onscreenClearValue = (VkClearColorValue){{0.5f, 0.5f, 0.5f, 1.0f}},
      .offscreenColorAttachmentCount = 0,
  };
}

void vulkan_pipeline_impl_skybox_record_render_pass(vulkan_pipeline *pipeline,
                                                    VkCommandBuffer commandBuffer,
                                                    size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->framebuffers, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;

  /* record new render pass into command buffer */
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = *framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

  // Draw cube.
  vulkan_renderer_cache_primitive_element *basicBoxPrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicBoxPrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicBoxPrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);

  vkCmdEndRenderPass(commandBuffer);
}

/* debug text */

vulkan_pipeline_info vulkan_pipeline_impl_debug_text_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .colorBlendingType = vulkan_color_blending_type_alpha};
}

void vulkan_pipeline_impl_debug_text_record_render_pass(vulkan_pipeline *pipeline,
                                                        VkCommandBuffer commandBuffer,
                                                        size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->framebuffers, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;

  /* record new render pass into command buffer */
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = *framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      pipeline->pipelineState->renderState->rendererCache->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);

  vkCmdEndRenderPass(commandBuffer);
}

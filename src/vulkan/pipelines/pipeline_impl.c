#include "pipeline_impl.h"
#include "pipeline.h"

/* forward */

vulkan_pipeline_info vulkan_pipeline_impl_forward_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                .offscreenColorAttachmentCount = 0,
                                .useDepthAttachment = true,
                                .depthClearValue = (VkClearDepthStencilValue){0.0f, 0}};
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

  vulkan_unified_geometry_buffer_record_bind_command(pipeline->renderState->unifiedGeometryBuffer,
                                                     commandBuffer);
  vulkan_descriptors_record_bind_command(pipeline->renderState->descriptors, commandBuffer,
                                         pipeline->pipelineLayout);
  vulkan_draw_push_constant_element drawPushConstant = {.currentFrameInFlight =
                                                            currentFrameInFlight};
  vkCmdPushConstants(commandBuffer, pipeline->pipelineLayout, VK_SHADER_STAGE_ALL, 0,
                     sizeof(drawPushConstant), &drawPushConstant);

  vulkan_batches_record_draw_command(pipeline->pipelineState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);

  vkCmdEndRenderPass(commandBuffer);
}

/* skybox */

// TODO: Implement skybox pipeline.

vulkan_pipeline_info vulkan_pipeline_impl_skybox_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.5f, 0.5f, 0.5f, 1.0f}},
                                .offscreenColorAttachmentCount = 0,
                                .useDepthAttachment = false};
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

  vulkan_unified_geometry_buffer_record_bind_command(pipeline->renderState->unifiedGeometryBuffer,
                                                     commandBuffer);
  vulkan_descriptors_record_bind_command(pipeline->renderState->descriptors, commandBuffer,
                                         pipeline->pipelineLayout);
  vulkan_draw_push_constant_element drawPushConstant = {.currentFrameInFlight =
                                                            currentFrameInFlight};
  vkCmdPushConstants(commandBuffer, pipeline->pipelineLayout, VK_SHADER_STAGE_ALL, 0,
                     sizeof(drawPushConstant), &drawPushConstant);

  // Draw cube.
  // HIRO HIRO functions to CmdDraw single primitive renderer cache without batching
  uint32_t indexOffset = pipeline->pipelineState->sharedState.skybox->boxRenderCache
                             ->vertexStreamElement.firstIndexOffset;
  uint32_t vertexOffset = pipeline->pipelineState->sharedState.skybox->boxRenderCache
                              ->vertexStreamElement.firstVertexOffset;
  vkCmdDrawIndexed(commandBuffer, 36, 1, indexOffset, vertexOffset, 0);

  vkCmdEndRenderPass(commandBuffer);
}

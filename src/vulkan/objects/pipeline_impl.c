#include "pipeline_impl.h"
#include "pipeline.h"

/* forward */

void vulkan_pipeline_impl_forward_get_framebuffer_attachment_count(vulkan_pipeline *pipeline,
                                                                   uint32_t *colorAttachmentCount,
                                                                   bool *useDepthAttachment) {
  *colorAttachmentCount = 1 /* swap chain image */;
  *useDepthAttachment = true;
}

void vulkan_pipeline_impl_forward_get_framebuffer_attachment_image_views(vulkan_pipeline *pipeline,
                                                                         size_t swapChainImageIdx,
                                                                         VkImageView *attachments) {
  attachments[0] =
      *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx);
  attachments[1] = pipeline->pipelineSharedState->depthBufferImage->imageView;
}

void vulkan_pipeline_impl_forward_get_framebuffer_attachment_clear_values(
    vulkan_pipeline *pipeline, VkClearValue *clearValues) {
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = (VkClearDepthStencilValue){0.0f, 0};
}

void vulkan_pipeline_impl_forward_get_render_pass_create_info(
    vulkan_pipeline *pipeline, VkAttachmentDescription *colorAttachmentDescriptions,
    VkAttachmentReference *colorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference) {
  VkAttachmentDescription colorAttachment = {0};
  colorAttachment.format = pipeline->vks->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  // HIRO HIRO chaining of layout with prev pipeline (in vulkan_pipeline.c?)

  VkAttachmentDescription depthAttachment = {0};
  depthAttachment.format = vulkan_find_depth_format(pipeline->vks->vkd);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {0};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {0};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  colorAttachmentDescriptions[0] = colorAttachment;
  colorAttachmentReferences[0] = colorAttachmentRef;
  *depthAttachmentDescription = depthAttachment;
  *depthAttachmentReference = depthAttachmentRef;
}

void vulkan_pipeline_impl_forward_record_render_pass(vulkan_pipeline *pipeline,
                                                     VkCommandBuffer commandBuffer,
                                                     size_t swapChainImageIdx) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;

  /* record new render pass into command buffer */
  // HIRO HIRO pass render pass and framebuffer as argument, allocate if NULL
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = frameState->framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount;
  vulkan_pipeline_get_framebuffer_attachment_count(pipeline, &clearValueCount, &(uint32_t){0},
                                                   &(bool){0});
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_get_framebuffer_attachment_clear_values(pipeline, clearValues);
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

  vulkan_batches_record_draw_command(pipeline->renderState->batches, commandBuffer,
                                     &frameState->batchesData);

  vkCmdEndRenderPass(commandBuffer);
}

/* skybox */

// TODO: Implement skybox pipeline.

void vulkan_pipeline_impl_skybox_get_framebuffer_attachment_count(vulkan_pipeline *pipeline,
                                                                  uint32_t *colorAttachmentCount,
                                                                  bool *useDepthAttachment) {
  *colorAttachmentCount = 1 /* swap chain image */;
  *useDepthAttachment = false;
}

void vulkan_pipeline_impl_skybox_get_framebuffer_attachment_image_views(vulkan_pipeline *pipeline,
                                                                        size_t swapChainImageIdx,
                                                                        VkImageView *attachments) {
  attachments[0] =
      *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx);
}

void vulkan_pipeline_impl_skybox_get_framebuffer_attachment_clear_values(
    vulkan_pipeline *pipeline, VkClearValue *clearValues) {
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
}

void vulkan_pipeline_impl_skybox_get_render_pass_create_info(
    vulkan_pipeline *pipeline, VkAttachmentDescription *colorAttachmentDescriptions,
    VkAttachmentReference *colorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference) {
  VkAttachmentDescription colorAttachment = {0};
  colorAttachment.format = pipeline->vks->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {0};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  colorAttachmentDescriptions[0] = colorAttachment;
  colorAttachmentReferences[0] = colorAttachmentRef;
}

void vulkan_pipeline_impl_skybox_record_render_pass(vulkan_pipeline *pipeline,
                                                    VkCommandBuffer commandBuffer,
                                                    size_t swapChainImageIdx) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;

  /* record new render pass into command buffer */
  // HIRO HIRO implement skybox
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = frameState->framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount;
  vulkan_pipeline_get_framebuffer_attachment_count(pipeline, &clearValueCount, &(uint32_t){0},
                                                   &(bool){0});
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_get_framebuffer_attachment_clear_values(pipeline, clearValues);
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

  vulkan_batches_record_draw_command(pipeline->renderState->batches, commandBuffer,
                                     &frameState->batchesData);

  vkCmdEndRenderPass(commandBuffer);
}

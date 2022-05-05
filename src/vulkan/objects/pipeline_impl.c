#include "pipeline_impl.h"
#include "pipeline.h"

uint32_t vulkan_pipeline_info_get_framebuffer_attachment_count(vulkan_pipeline_info pipelineInfo) {
  uint32_t count = (pipelineInfo.useOnscreenColorAttachment ? 1 : 0) +
                   pipelineInfo.offscreenColorAttachmentCount +
                   (pipelineInfo.useDepthAttachment ? 1 : 0);
  assert(count > 0);
  return count;
}

void vulkan_pipeline_info_get_framebuffer_attachment_image_views(
    vulkan_pipeline_info pipelineInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments) {
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {
    assert(swapChainImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = swapChainImageView;
  }
  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {
    framebufferAttachments[idx++] = offscreenImageViews[i];
  }
  if (pipelineInfo.useDepthAttachment) {
    assert(depthBufferImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = depthBufferImageView;
  }
}

void vulkan_pipeline_info_get_framebuffer_attachment_clear_values(vulkan_pipeline_info pipelineInfo,
                                                                  VkClearValue *clearValues) {
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {
    clearValues[idx++].color = pipelineInfo.onscreenClearValue;
  }
  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {
    clearValues[idx++].color = pipelineInfo.offscreenClearValue;
  }
  if (pipelineInfo.useDepthAttachment) {
    clearValues[idx++].depthStencil = pipelineInfo.depthClearValue;
  }
}

void vulkan_pipeline_info_get_render_pass_create_info(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline *prev, vulkan_pipeline *next,
    VkFormat swapChainImageFormat, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference) {

  /* analyze pipeline chain */
  bool isFirstPipeline = (prev == NULL);
  bool isLastPipeline = (next == NULL);
  size_t prevOnscreenColorAttachmentCount = 0;
  size_t nextOnscreenColorAttachmentCount = 0;
  vulkan_pipeline *it = prev;
  while (it != NULL) {
    vulkan_pipeline_info info = vulkan_pipeline_get_pipeline_info(it);
    if (info.useOnscreenColorAttachment) {
      prevOnscreenColorAttachmentCount++;
    }
    it = it->prev;
  }
  it = next;
  while (it != NULL) {
    vulkan_pipeline_info info = vulkan_pipeline_get_pipeline_info(it);
    if (info.useOnscreenColorAttachment) {
      nextOnscreenColorAttachmentCount++;
    }
    it = it->next;
  }
  bool isFirstOnscreenPipeline =
      pipelineInfo.useOnscreenColorAttachment && prevOnscreenColorAttachmentCount == 0;
  bool isLastOnscreenPipeline =
      pipelineInfo.useOnscreenColorAttachment && nextOnscreenColorAttachmentCount == 0;

  /* prepare attachment info */
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {

    VkAttachmentLoadOp loadOp;
    if (isFirstOnscreenPipeline) {
      loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    } else {
      loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    VkImageLayout initialLayout;
    if (isFirstOnscreenPipeline) {
      initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    } else {
      initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkImageLayout finalLayout;
    if (isLastOnscreenPipeline) {
      finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } else {
      finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    *onscreenColorAttachmentDescription = (VkAttachmentDescription){
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = loadOp,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = initialLayout,
        .finalLayout = finalLayout,
    };
    *onscreenColorAttachmentReference = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
  }

  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {
    offscreenColorAttachmentDescriptions[i] = (VkAttachmentDescription){
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // HIRO HIRO or load
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // HIRO HIRO or color optimal
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    offscreenColorAttachmentReferences[i] = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
  }

  if (pipelineInfo.useDepthAttachment) {
    *depthAttachmentDescription = (VkAttachmentDescription){
        .format = depthBufferImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    *depthAttachmentReference = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
  }
}

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
  vulkan_pipeline_framebuffer_state *framebufferState =
      utarray_eltptr(pipeline->framebufferStates, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  // HIRO pass render pass and framebuffer as argument, allocate if NULL
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = framebufferState->framebuffer;
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

  vulkan_batches_record_draw_command(pipeline->renderState->batches, commandBuffer,
                                     &frameState->batchesData);

  vkCmdEndRenderPass(commandBuffer);
}

/* skybox */

// TODO: Implement skybox pipeline.

vulkan_pipeline_info vulkan_pipeline_impl_skybox_get_pipeline_info(vulkan_pipeline *pipeline) {
  return (vulkan_pipeline_info){.useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                .offscreenColorAttachmentCount = 0,
                                .useDepthAttachment = false};
}

void vulkan_pipeline_impl_skybox_record_render_pass(vulkan_pipeline *pipeline,
                                                    VkCommandBuffer commandBuffer,
                                                    size_t swapChainImageIdx) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  vulkan_pipeline_framebuffer_state *framebufferState =
      utarray_eltptr(pipeline->framebufferStates, swapChainImageIdx);
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  // HIRO implement skybox
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = framebufferState->framebuffer;
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

  vulkan_batches_record_draw_command(pipeline->renderState->batches, commandBuffer,
                                     &frameState->batchesData);

  vkCmdEndRenderPass(commandBuffer);
}

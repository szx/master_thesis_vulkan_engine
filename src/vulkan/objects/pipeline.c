#include "pipeline.h"

void create_frame_states(vulkan_pipeline *pipeline) {
  utarray_alloc(pipeline->frameStates, sizeof(vulkan_pipeline_frame_state));
  utarray_resize(pipeline->frameStates, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_init(frameState, pipeline, swapChainImageIdx);
    swapChainImageIdx++;
  }
}

void destroy_frame_states(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_deinit(frameState);
  }
  utarray_free(pipeline->frameStates);
}

void create_render_pass(vulkan_pipeline *pipeline) {
  VkAttachmentDescription colorAttachment = {0};
  colorAttachment.format = pipeline->vks->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

  VkAttachmentDescription colorAttachmentDescriptions[] = {colorAttachment};
  VkAttachmentReference colorAttachmentReferences[] = {colorAttachmentRef};

  pipeline->renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, colorAttachmentDescriptions, array_size(colorAttachmentDescriptions),
      colorAttachmentReferences, array_size(colorAttachmentReferences), depthAttachment,
      depthAttachmentRef, "pipeline");
}

void destroy_render_pass(vulkan_pipeline *pipeline) {
  vkDestroyRenderPass(pipeline->vks->vkd->device, pipeline->renderPass, vka);
}

void create_graphics_pipeline(vulkan_pipeline *pipeline) {
  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  pipeline->graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd, pipeline->shaderProgram, pipeline->renderState, pipeline->vks,
      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, &pipeline->pipelineLayout, "pipeline");

  core_free(descriptorSetLayouts);
}

void destroy_graphics_pipeline(vulkan_pipeline *pipeline) {
  vkDestroyPipelineLayout(pipeline->vks->vkd->device, pipeline->pipelineLayout, vka);
  vkDestroyPipeline(pipeline->vks->vkd->device, pipeline->graphicsPipeline, vka);
}

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                        vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));
  vulkan_pipeline_init(pipeline, vks, renderState, pipelineSharedState);
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_pipeline_deinit(pipeline);
  core_free(pipeline);
}

void vulkan_pipeline_init(vulkan_pipeline *pipeline, vulkan_swap_chain *vks,
                          vulkan_render_state *renderState,
                          vulkan_pipeline_shared_state *pipelineSharedState) {
  pipeline->vks = vks;
  pipeline->renderState = renderState;
  pipeline->pipelineSharedState = pipelineSharedState;

  pipeline->shaderProgram = vulkan_shader_program_create(renderState);
  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);

  create_frame_states(pipeline);
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) {
  destroy_frame_states(pipeline);
  destroy_graphics_pipeline(pipeline);
  destroy_render_pass(pipeline);
  vulkan_shader_program_destroy(pipeline->shaderProgram);
}

size_t vulkan_pipeline_get_framebuffer_attachment_count(vulkan_pipeline *pipeline) {
  size_t attachmentCount = 1 + 1;
  return attachmentCount;
}

void vulkan_pipeline_get_framebuffer_attachments(vulkan_pipeline *pipeline,
                                                 size_t swapChainImageIdx,
                                                 VkImageView *attachments) {
  attachments[0] =
      *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx);
  attachments[1] = pipeline->pipelineSharedState->depthBufferImage->imageView;
}

void vulkan_pipeline_get_framebuffer_attachment_clear_values(vulkan_pipeline *pipeline,
                                                             VkClearValue *clearValues) {
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = (VkClearDepthStencilValue){0.0f, 0};
}

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline, size_t swapChainImageIdx) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, swapChainImageIdx);

  vulkan_pipeline_frame_state_send_to_device(frameState);
}

void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
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
  size_t clearValueCount = vulkan_pipeline_get_framebuffer_attachment_count(pipeline);
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

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("pipeline:\n");
  vulkan_shader_program_debug_print(pipeline->shaderProgram, 2);
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_debug_print(frameState, 2);
  }
  vulkan_pipeline_shared_state_debug_print(pipeline->pipelineSharedState, 2);
}

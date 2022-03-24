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
      pipeline->renderState->unifiedUniformBuffer->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 0;
  const VkPushConstantRange *pushConstantRanges = NULL;

  pipeline->graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd, pipeline->shaderGenerator, pipeline->renderState, pipeline->vks,
      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, &pipeline->pipelineLayout, "pipeline");

  core_free(descriptorSetLayouts);
}

void destroy_graphics_pipeline(vulkan_pipeline *pipeline) {
  vkDestroyPipelineLayout(pipeline->vks->vkd->device, pipeline->pipelineLayout, vka);
  vkDestroyPipeline(pipeline->vks->vkd->device, pipeline->graphicsPipeline, vka);
}

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  pipeline->vks = vks;
  pipeline->renderState = renderState;

  pipeline->shaderGenerator = vulkan_shader_generator_create(renderState);
  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);

  pipeline->sharedState = vulkan_pipeline_shared_state_create(pipeline);
  create_frame_states(pipeline);

  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  destroy_frame_states(pipeline);
  vulkan_pipeline_shared_state_destroy(pipeline->sharedState);
  destroy_graphics_pipeline(pipeline);
  destroy_render_pass(pipeline);
  vulkan_shader_generator_destroy(pipeline->shaderGenerator);
  core_free(pipeline);
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
  attachments[1] = pipeline->sharedState->depthBufferImage->imageView;
}

void vulkan_pipeline_get_framebuffer_attachment_clear_values(vulkan_pipeline *pipeline,
                                                             VkClearValue *clearValues) {
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = (VkClearDepthStencilValue){0.0f, 0};
}

VkCommandBuffer vulkan_pipeline_record_command_buffer(vulkan_pipeline *pipeline,
                                                      size_t swapChainImageIdx) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, swapChainImageIdx);

  VkCommandBuffer commandBuffer = frameState->commandBuffer;
  vkResetCommandBuffer(commandBuffer, 0);

  /* record command buffer */
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  verify(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);

  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipeline->renderPass;
  renderPassInfo.framebuffer = frameState->swapChainFramebuffer;
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
  vulkan_unified_uniform_buffer_record_bind_command(pipeline->renderState->unifiedUniformBuffer,
                                                    commandBuffer, pipeline->pipelineLayout);

  dl_foreach_elem(vulkan_batch *, batch, pipeline->renderState->batches->batches,
                  { vulkan_batch_record_draw_command(batch, commandBuffer); })

  vkCmdEndRenderPass(commandBuffer);

  verify(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);

  return commandBuffer;
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("pipeline:\n");
  vulkan_shader_generator_debug_print(pipeline->shaderGenerator, 2);
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_debug_print(frameState, 2);
  }
  vulkan_pipeline_shared_state_debug_print(pipeline->sharedState, 2);
}

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline, uint32_t swapChainImageIdx) {
  frameState->pipeline = pipeline;

  frameState->swapChainImageIdx = swapChainImageIdx;

  size_t framebufferAttachmentCount =
      vulkan_pipeline_get_framebuffer_attachment_count(frameState->pipeline);
  assert(framebufferAttachmentCount > 0);
  VkImageView framebufferAttachments[framebufferAttachmentCount];
  vulkan_pipeline_get_framebuffer_attachments(frameState->pipeline, frameState->swapChainImageIdx,
                                              framebufferAttachments);

  frameState->swapChainFramebuffer = vulkan_create_framebuffer(
      frameState->pipeline->vks->vkd, frameState->pipeline->renderPass, framebufferAttachmentCount,
      framebufferAttachments, frameState->pipeline->vks->swapChainExtent.width,
      frameState->pipeline->vks->swapChainExtent.height, "frame state #%d",
      frameState->swapChainImageIdx);

  vulkan_queue_families queueFamilies = find_queue_families(
      frameState->pipeline->vks->vkd, frameState->pipeline->vks->vkd->physicalDevice);
  frameState->commandPool = vulkan_create_command_pool(
      frameState->pipeline->vks->vkd, queueFamilies.graphicsFamily,
      VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
          VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // command buffer is short-lived,
                                                           // rerecorded every frame
      "frame state #%d", frameState->swapChainImageIdx);

  frameState->commandBuffer =
      vulkan_create_command_buffer(frameState->pipeline->vks->vkd, frameState->commandPool,
                                   "frame state #%d", frameState->swapChainImageIdx);
}

void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState) {
  vkFreeCommandBuffers(frameState->pipeline->vks->vkd->device, frameState->commandPool, 1,
                       &frameState->commandBuffer);
  vkDestroyCommandPool(frameState->pipeline->vks->vkd->device, frameState->commandPool, vka);
  vkDestroyFramebuffer(frameState->pipeline->vks->vkd->device, frameState->swapChainFramebuffer,
                       vka);
}

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent) {
  log_debug(INDENT_FORMAT_STRING "frame state #%d:", INDENT_FORMAT_ARGS(0),
            frameState->swapChainImageIdx);
}

vulkan_pipeline_shared_state *vulkan_pipeline_shared_state_create(vulkan_pipeline *pipeline) {
  vulkan_pipeline_shared_state *sharedState = core_alloc(sizeof(vulkan_pipeline_shared_state));

  sharedState->pipeline = pipeline;

  vulkan_swap_chain *vks = sharedState->pipeline->vks;
  sharedState->depthBufferImage =
      vulkan_image_create(vks->vkd, vulkan_image_type_depth_buffer, vks->swapChainExtent.width,
                          vks->swapChainExtent.height);

  return sharedState;
}

void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *sharedState) {
  vulkan_image_destroy(sharedState->depthBufferImage);
  core_free(sharedState);
}

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *sharedState,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_image_debug_print(sharedState->depthBufferImage, indent + 2);
}

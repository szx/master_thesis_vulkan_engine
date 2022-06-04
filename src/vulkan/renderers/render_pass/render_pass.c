#include "render_pass.h"

size_t get_cache_len(vulkan_render_pass *renderPass) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(renderPass->renderState->vks->swapChainImages);
  return frameInFlightCount * swapChainImageCount;
}

size_t get_cache_index(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                       size_t swapChainImageIdx) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(renderPass->renderState->vks->swapChainImages);
  if (frameInFlightCount < swapChainImageCount) {
    return swapChainImageIdx * frameInFlightCount + currentFrameInFlight;
  }
  if (frameInFlightCount >= swapChainImageCount) {
    return currentFrameInFlight * swapChainImageCount + swapChainImageIdx;
  }
  UNREACHABLE;
}

VkPipeline get_graphics_pipeline(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                                 size_t swapChainImageIdx, vulkan_rendering_info renderingInfo) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkPipeline *graphicsPipeline = utarray_eltptr(renderPass->_graphicsPipelines, cacheIndex);
  if (*graphicsPipeline != VK_NULL_HANDLE) {
    return *graphicsPipeline;
  }

  uint32_t colorAttachmentCount = renderPass->desc.colorAttachmentCount;
  bool colorBlendingType = renderPass->desc.colorBlendingType;

  bool depthWriteEnable = renderPass->desc.offscreenDepthAttachment.name &&
                          renderPass->desc.offscreenDepthAttachment.depthWriteEnable;
  bool depthTestEnable = renderPass->desc.offscreenDepthAttachment.name &&
                         renderPass->desc.offscreenDepthAttachment.depthTestEnable;
  VkCompareOp depthTestOp = renderPass->desc.offscreenDepthAttachment.depthTestOp;

  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_render_pass_shader_program_get_shader_stages(renderPass->shaderProgram,
                                                          &shaderStageCount);

  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      renderPass->renderState->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  uint32_t vertexBindingDescriptionsCount =
      vulkan_vertex_stream_get_vertex_buffer_binding_count(renderPass->renderState->vertexStream);
  assert(vertexBindingDescriptionsCount == 1);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_vertex_stream_get_vertex_buffer_binding_description(
          renderPass->renderState->vertexStream);

  uint32_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_vertex_stream_get_vertex_attribute_descriptions(renderPass->renderState->vertexStream,
                                                             &vertexAttributeDescriptionsCount);

  vulkan_graphics_pipeline_create_info createInfo = {
      .colorAttachmentCount = colorAttachmentCount,
      .colorBlendingType = colorBlendingType,

      .depthWriteEnable = depthWriteEnable,
      .depthTestEnable = depthTestEnable,
      .depthTestOp = depthTestOp,

      .shaderStages = shaderStages,
      .shaderStageCount = shaderStageCount,

      .vertexInputBindingDescriptions = &vertexInputBindingDescription,
      .vertexBindingDescriptionsCount = vertexBindingDescriptionsCount,
      .vertexAttributeDescriptions = vertexAttributeDescriptions,
      .vertexAttributeDescriptionsCount = vertexAttributeDescriptionsCount,

      .framebufferWidth = renderPass->renderState->vks->swapChainExtent.width,
      .framebufferHeight = renderPass->renderState->vks->swapChainExtent.height,

      .descriptorSetLayouts = descriptorSetLayouts,
      .descriptorSetLayoutCount = descriptorSetLayoutCount,
      .pushConstantRanges = pushConstantRanges,
      .pushConstantRangeCount = pushConstantRangeCount,

      .renderPass = NULL,
      .pipelineLayout = renderPass->renderState->descriptors->pipelineLayout,
  };

  *graphicsPipeline =
      vulkan_create_graphics_pipeline(renderPass->renderState->vkd, createInfo, renderingInfo,
                                      "renderPass (frameInFlight=#%u, swapChainImage=#%u)",
                                      currentFrameInFlight, swapChainImageIdx);

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);
  core_free(shaderStages);

  return *graphicsPipeline;
}

vulkan_render_pass *vulkan_render_pass_create(vulkan_render_pass_desc desc,
                                              vulkan_render_state *renderState,
                                              vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass *renderPass = core_alloc(sizeof(vulkan_render_pass));
  vulkan_render_pass_init(renderPass, desc, renderState, renderPassState);
  return renderPass;
}

void vulkan_render_pass_destroy(vulkan_render_pass *renderPass) {
  vulkan_render_pass_deinit(renderPass);
  core_free(renderPass);
}

void vulkan_render_pass_init(vulkan_render_pass *renderPass, vulkan_render_pass_desc desc,
                             vulkan_render_state *renderState,
                             vulkan_render_pass_state *renderPassState) {
  renderPass->renderState = renderState;
  renderPass->renderPassState = renderPassState;

  renderPass->desc = desc;
  renderPass->shaderProgram =
      vulkan_render_pass_shader_program_create(renderState, renderPass->desc);

  /* cache */
  utarray_alloc(renderPass->_graphicsPipelines, sizeof(VkPipeline));
  utarray_resize(renderPass->_graphicsPipelines, get_cache_len(renderPass));
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, renderPass->_graphicsPipelines) {
    *graphicsPipeline = VK_NULL_HANDLE;
  }
}

void vulkan_render_pass_deinit(vulkan_render_pass *renderPass) {
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, renderPass->_graphicsPipelines) {
    vkDestroyPipeline(renderPass->renderState->vkd->device, *graphicsPipeline, vka);
  }
  utarray_free(renderPass->_graphicsPipelines);
  vulkan_render_pass_shader_program_destroy(renderPass->shaderProgram);
}

void set_rendering_info_images(vulkan_render_pass_frame_state *frameState, size_t swapChainImageIdx,
                               vulkan_rendering_info *renderPassInfo) {
  if (renderPassInfo->onscreenColorAttachment) {
    VkImage swapChainImage = *(VkImage *)utarray_eltptr(
        frameState->renderPassState->renderState->vks->swapChainImages, swapChainImageIdx);
    renderPassInfo->onscreenColorAttachment->image = swapChainImage;
    VkImageView swapChainImageView = *(VkImageView *)utarray_eltptr(
        frameState->renderPassState->renderState->vks->swapChainImageViews, swapChainImageIdx);
    renderPassInfo->onscreenColorAttachment->imageView = swapChainImageView;
    renderPassInfo->onscreenColorAttachment->imageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
  }
  for (size_t i = 0; i < utarray_len(renderPassInfo->offscreenColorAttachments); i++) {
    vulkan_rendering_attachment_info *attachmentInfo =
        utarray_eltptr(renderPassInfo->offscreenColorAttachments, i);
    vulkan_image *image = vulkan_render_pass_offscreen_texture_state_get_offscreen_texture(
                              frameState->offscreenTextures, attachmentInfo->name)
                              ->image;
    attachmentInfo->image = image->image;
    attachmentInfo->imageView = image->imageView;
    attachmentInfo->imageAspectFlags = image->aspectFlags;
  }
  if (renderPassInfo->depthAttachment) {
    vulkan_image *image = vulkan_render_pass_offscreen_texture_state_get_offscreen_texture(
                              frameState->offscreenTextures, renderPassInfo->depthAttachment->name)
                              ->image;
    renderPassInfo->depthAttachment->image = image->image;
    renderPassInfo->depthAttachment->imageView = image->imageView;
    renderPassInfo->depthAttachment->imageAspectFlags = image->aspectFlags;
  }
}

void vulkan_render_pass_record_commands(vulkan_render_pass *renderPass,
                                        VkCommandBuffer commandBuffer, size_t swapChainImageIdx,
                                        vulkan_rendering_info renderPassInfo) {
  size_t currentFrameInFlight = renderPass->renderState->sync->currentFrameInFlight;
  vulkan_render_pass_frame_state *frameState =
      vulkan_render_pass_state_get_frame_state(renderPass->renderPassState, currentFrameInFlight);

  set_rendering_info_images(frameState, swapChainImageIdx, &renderPassInfo);

  VkPipeline _graphicsPipeline =
      get_graphics_pipeline(renderPass, currentFrameInFlight, swapChainImageIdx, renderPassInfo);

  /* record new render pass into command buffer */
  vulkan_begin_rendering(renderPass->renderState->vkd, commandBuffer, renderPassInfo);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

  renderPass->desc.recordFunc(renderPass, frameState, commandBuffer);

  vulkan_end_rendering(renderPass->renderState->vkd, commandBuffer, renderPassInfo);
}

void vulkan_render_pass_debug_print(vulkan_render_pass *renderPass) {
  log_debug("renderPass:\n");
  vulkan_render_pass_shader_program_debug_print(renderPass->shaderProgram, 2);
  vulkan_render_pass_state_debug_print(renderPass->renderPassState, 2);
}

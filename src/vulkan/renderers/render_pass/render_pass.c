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

VkRenderPass get_render_pass(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                             size_t swapChainImageIdx, vulkan_render_pass_info createInfo) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass *_renderPass = utarray_eltptr(renderPass->_renderPasses, cacheIndex);
  if (*_renderPass != VK_NULL_HANDLE) {
    return *_renderPass;
  }

  *_renderPass = vulkan_create_render_pass(renderPass->renderState->vkd, createInfo,
                                           "renderPass (frameInFlight=#%u, swapChainImage=#%u)",
                                           currentFrameInFlight, swapChainImageIdx);

  return *_renderPass;
}

VkFramebuffer get_framebuffer(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                              size_t swapChainImageIdx,

                              uint32_t framebufferAttachmentCount,
                              VkImageView *framebufferAttachments, VkRenderPass _renderPass) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkFramebuffer *_framebuffer = utarray_eltptr(renderPass->_framebuffers, cacheIndex);
  if (*_framebuffer != VK_NULL_HANDLE) {
    return *_framebuffer;
  }

  assert(framebufferAttachmentCount > 0);

  *_framebuffer = vulkan_create_framebuffer(renderPass->renderState->vkd, _renderPass,
                                            framebufferAttachmentCount, framebufferAttachments,
                                            renderPass->renderState->vks->swapChainExtent.width,
                                            renderPass->renderState->vks->swapChainExtent.height,
                                            "framebuffer (frameInFlight=#%u, swapChainImage=#%u)",
                                            currentFrameInFlight, swapChainImageIdx);

  return *_framebuffer;
}

VkPipeline get_graphics_pipeline(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                                 size_t swapChainImageIdx, VkRenderPass _renderPass) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkPipeline *graphicsPipeline = utarray_eltptr(renderPass->_graphicsPipelines, cacheIndex);
  if (*graphicsPipeline != VK_NULL_HANDLE) {
    return *graphicsPipeline;
  }

  uint32_t colorAttachmentCount = renderPass->desc.colorAttachmentCount;
  bool colorBlendingType = renderPass->desc.colorBlendingType;

  bool depthWriteEnable =
      renderPass->desc.useDepthAttachment && renderPass->desc.depthAttachmentWriteEnable;
  bool depthTestEnable =
      renderPass->desc.useDepthAttachment && renderPass->desc.depthAttachmentTestEnable;
  VkCompareOp depthTestOp = renderPass->desc.depthAttachmentTestOp;

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

      .renderPass = _renderPass,
      .pipelineLayout = renderPass->renderState->descriptors->pipelineLayout,
  };

  *graphicsPipeline =
      vulkan_create_graphics_pipeline(renderPass->renderState->vkd, createInfo,
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
  vulkan_render_pass_desc_calculate_additional_info(&renderPass->desc, renderPass);
  renderPass->shaderProgram =
      vulkan_render_pass_shader_program_create(renderState, renderPass->desc);

  /* cache */
  utarray_alloc(renderPass->_renderPasses, sizeof(VkRenderPass));
  utarray_resize(renderPass->_renderPasses, get_cache_len(renderPass));
  utarray_foreach_elem_it (VkRenderPass *, _renderPass, renderPass->_renderPasses) {
    *_renderPass = VK_NULL_HANDLE;
  }

  utarray_alloc(renderPass->_framebuffers, sizeof(VkFramebuffer));
  utarray_resize(renderPass->_framebuffers, get_cache_len(renderPass));
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, renderPass->_framebuffers) {
    *framebuffer = VK_NULL_HANDLE;
  }

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
  utarray_foreach_elem_it (VkRenderPass *, _renderPass, renderPass->_renderPasses) {
    vkDestroyRenderPass(renderPass->renderState->vkd->device, *_renderPass, vka);
  }
  utarray_free(renderPass->_renderPasses);
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, renderPass->_framebuffers) {
    vkDestroyFramebuffer(renderPass->renderState->vkd->device, *framebuffer, vka);
  }
  utarray_free(renderPass->_framebuffers);
  vulkan_render_pass_shader_program_destroy(renderPass->shaderProgram);
}

void get_framebuffer_attachment_image_views(vulkan_render_pass *renderPass,
                                            vulkan_render_pass_frame_state *frameState,
                                            size_t swapChainImageIdx,
                                            VkImageView *framebufferAttachmentImageViews) {
  size_t i = 0;
  if (renderPass->desc.useOnscreenColorAttachment) {
    VkImageView swapChainImageView = *(VkImageView *)utarray_eltptr(
        renderPass->renderState->vks->swapChainImageViews, swapChainImageIdx);
    framebufferAttachmentImageViews[i++] = swapChainImageView;
  }
  for (size_t j = 0; j < renderPass->desc.offscreenColorAttachmentCount; j++) {
    framebufferAttachmentImageViews[i++] =
        vulkan_render_pass_frame_state_get_offscreen_texture(
            frameState, renderPass->desc.offscreenColorAttachments[j].type)
            ->image->imageView;
  }
  if (renderPass->desc.useDepthAttachment) {
    VkImageView depthBufferImageView = frameState->depthBufferImage->imageView;
    framebufferAttachmentImageViews[i++] = depthBufferImageView;
  }
}

void vulkan_render_pass_record_commands(vulkan_render_pass *renderPass,
                                        VkCommandBuffer commandBuffer, size_t swapChainImageIdx,
                                        vulkan_render_pass_info renderPassInfo) {
  size_t currentFrameInFlight = renderPass->renderState->sync->currentFrameInFlight;
  vulkan_render_pass_frame_state *frameState =
      vulkan_render_pass_state_get_frame_state(renderPass->renderPassState, currentFrameInFlight);

  uint32_t framebufferAttachmentCount = renderPass->desc.attachmentCount;
  VkImageView framebufferAttachmentImageViews[VLA(framebufferAttachmentCount)];
  get_framebuffer_attachment_image_views(renderPass, frameState, swapChainImageIdx,
                                         framebufferAttachmentImageViews);

  VkRenderPass _renderPass =
      get_render_pass(renderPass, currentFrameInFlight, swapChainImageIdx, renderPassInfo);
  VkFramebuffer _framebuffer =
      get_framebuffer(renderPass, currentFrameInFlight, swapChainImageIdx,
                      framebufferAttachmentCount, framebufferAttachmentImageViews, _renderPass);
  VkPipeline _graphicsPipeline =
      get_graphics_pipeline(renderPass, currentFrameInFlight, swapChainImageIdx, _renderPass);

  /* record new render pass into command buffer */

  vulkan_begin_render_pass(renderPass->renderState->vkd, commandBuffer, renderPassInfo, _renderPass,
                           _framebuffer);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

  renderPass->desc.recordFunc(renderPass, frameState, commandBuffer);

  vulkan_end_render_pass(renderPass->renderState->vkd, commandBuffer);
}

void vulkan_render_pass_debug_print(vulkan_render_pass *renderPass) {
  log_debug("renderPass:\n");
  vulkan_render_pass_shader_program_debug_print(renderPass->shaderProgram, 2);
  vulkan_render_pass_state_debug_print(renderPass->renderPassState, 2);
}

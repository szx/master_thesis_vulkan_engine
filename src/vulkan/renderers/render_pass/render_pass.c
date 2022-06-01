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
                             size_t swapChainImageIdx, vulkan_render_pass_create_info createInfo) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass *_renderPass = utarray_eltptr(renderPass->_renderPasses, cacheIndex);
  if (*_renderPass != VK_NULL_HANDLE) {
    return *_renderPass;
  }

  *_renderPass = vulkan_create_render_pass(renderPass->renderState->vkd, createInfo,
                                           "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
                                           vulkan_render_pass_type_debug_str(renderPass->type),
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

  *_framebuffer = vulkan_create_framebuffer(
      renderPass->renderState->vkd, _renderPass, framebufferAttachmentCount, framebufferAttachments,
      renderPass->renderState->vks->swapChainExtent.width,
      renderPass->renderState->vks->swapChainExtent.height,
      "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_render_pass_type_debug_str(renderPass->type), currentFrameInFlight, swapChainImageIdx);

  return *_framebuffer;
}

VkPipeline get_graphics_pipeline(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                                 size_t swapChainImageIdx, VkRenderPass _renderPass) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkPipeline *graphicsPipeline = utarray_eltptr(renderPass->_graphicsPipelines, cacheIndex);
  if (*graphicsPipeline != VK_NULL_HANDLE) {
    return *graphicsPipeline;
  }

  uint32_t colorAttachmentCount = renderPass->info._framebufferColorAttachmentCount;
  bool colorBlendingType = renderPass->info.colorBlendingType;

  bool depthWriteEnable =
      renderPass->info.useDepthAttachment && renderPass->info.depthAttachmentWriteEnable;
  bool depthTestEnable =
      renderPass->info.useDepthAttachment && renderPass->info.depthAttachmentTestEnable;
  VkCompareOp depthTestOp = renderPass->info.depthAttachmentTestOp;

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

  *graphicsPipeline = vulkan_create_graphics_pipeline(
      renderPass->renderState->vkd, createInfo,
      "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_render_pass_type_debug_str(renderPass->type), currentFrameInFlight, swapChainImageIdx);

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);
  core_free(shaderStages);

  return *graphicsPipeline;
}

vulkan_render_pass *vulkan_render_pass_create(vulkan_render_pass_type type,
                                              vulkan_render_state *renderState,
                                              vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass *renderPass = core_alloc(sizeof(vulkan_render_pass));
  vulkan_render_pass_init(renderPass, type, renderState, renderPassState);
  return renderPass;
}

void vulkan_render_pass_destroy(vulkan_render_pass *renderPass) {
  vulkan_render_pass_deinit(renderPass);
  core_free(renderPass);
}

void vulkan_render_pass_init(vulkan_render_pass *renderPass, vulkan_render_pass_type type,
                             vulkan_render_state *renderState,
                             vulkan_render_pass_state *renderPassState) {
  renderPass->renderState = renderState;
  renderPass->renderPassState = renderPassState;

  renderPass->type = type;
  renderPass->info = vulkan_render_pass_get_info(renderPass);
  renderPass->shaderProgram =
      vulkan_render_pass_shader_program_create(renderState, renderPass->info);

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
  utarray_foreach_elem_it (VkRenderPass *, _renderPass, renderPass->_renderPasses) {
    vkDestroyRenderPass(renderPass->renderState->vkd->device, *_renderPass, vka);
  }
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, renderPass->_framebuffers) {
    vkDestroyFramebuffer(renderPass->renderState->vkd->device, *framebuffer, vka);
  }
  vulkan_render_pass_shader_program_destroy(renderPass->shaderProgram);
}

vulkan_render_pass_info vulkan_render_pass_get_info(vulkan_render_pass *renderPass) {
#define x(_name, ...)                                                                              \
  if (renderPass->type == vulkan_render_pass_type_##_name) {                                       \
    vulkan_render_pass_info renderPassInfo =                                                       \
        vulkan_render_pass_impl_##_name##_get_info(renderPass);                                    \
    vulkan_render_pass_info_calculate_additional_info(&renderPassInfo, renderPass);                \
    return renderPassInfo;                                                                         \
  }
  VULKAN_RENDER_PASS_TYPES(x, )
#undef x
  UNREACHABLE;
}

void get_framebuffer_attachment_info(vulkan_render_pass *renderPass,
                                     uint32_t *framebufferAttachmentCount,
                                     VkClearValue **framebufferAttachmentClearValues) {
  *framebufferAttachmentCount = renderPass->info._framebufferAttachmentCount;
  *framebufferAttachmentClearValues = renderPass->info._framebufferAttachmentClearValues;
}

void get_framebuffer_attachment_image_views(vulkan_render_pass *renderPass,
                                            vulkan_render_pass_frame_state *frameState,
                                            size_t swapChainImageIdx,
                                            VkImageView *framebufferAttachmentImageViews) {
  size_t i = 0;
  if (renderPass->info.useOnscreenColorAttachment) {
    VkImageView swapChainImageView = *(VkImageView *)utarray_eltptr(
        renderPass->renderState->vks->swapChainImageViews, swapChainImageIdx);
    framebufferAttachmentImageViews[i++] = swapChainImageView;
  }
  for (size_t j = 0; j < renderPass->info.offscreenColorAttachmentCount; j++) {
    framebufferAttachmentImageViews[i++] =
        vulkan_render_pass_frame_state_get_offscreen_texture(
            frameState, renderPass->info.offscreenColorAttachments[j].type)
            ->image->imageView;
  }
  if (renderPass->info.useDepthAttachment) {
    VkImageView depthBufferImageView = frameState->depthBufferImage->imageView;
    framebufferAttachmentImageViews[i++] = depthBufferImageView;
  }
}

void vulkan_render_pass_record_commands(vulkan_render_pass *renderPass,
                                        VkCommandBuffer commandBuffer, size_t swapChainImageIdx,
                                        vulkan_render_pass_create_info renderPassCreateInfo) {
  size_t currentFrameInFlight = renderPass->renderState->sync->currentFrameInFlight;
  vulkan_render_pass_frame_state *frameState =
      vulkan_render_pass_state_get_frame_state(renderPass->renderPassState, currentFrameInFlight);

  uint32_t framebufferAttachmentCount;
  VkClearValue *framebufferAttachmentClearValues;
  get_framebuffer_attachment_info(renderPass, &framebufferAttachmentCount,
                                  &framebufferAttachmentClearValues);
  VkImageView framebufferAttachmentImageViews[VLA(framebufferAttachmentCount)];
  get_framebuffer_attachment_image_views(renderPass, frameState, swapChainImageIdx,
                                         framebufferAttachmentImageViews);

  VkRenderPass _renderPass =
      get_render_pass(renderPass, currentFrameInFlight, swapChainImageIdx, renderPassCreateInfo);
  VkFramebuffer _framebuffer =
      get_framebuffer(renderPass, currentFrameInFlight, swapChainImageIdx,
                      framebufferAttachmentCount, framebufferAttachmentImageViews, _renderPass);
  VkPipeline _graphicsPipeline =
      get_graphics_pipeline(renderPass, currentFrameInFlight, swapChainImageIdx, _renderPass);

  /* record new render pass into command buffer */
  // HIRO move to device_functions, get clear values from vulkan_render_pass_create_info
  VkRenderPassBeginInfo renderPassBeginInfo = {0};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = _renderPass;
  renderPassBeginInfo.framebuffer = _framebuffer;
  renderPassBeginInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassBeginInfo.renderArea.extent = renderPass->renderState->vks->swapChainExtent;
  renderPassBeginInfo.clearValueCount = framebufferAttachmentCount;
  renderPassBeginInfo.pClearValues = framebufferAttachmentClearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

#define x(_name, ...)                                                                              \
  if (renderPass->type == vulkan_render_pass_type_##_name) {                                       \
    vulkan_render_pass_impl_##_name##_record_commands(renderPass, frameState, commandBuffer);      \
  }
  VULKAN_RENDER_PASS_TYPES(x, )
#undef x

  vkCmdEndRenderPass(commandBuffer);
}

void vulkan_render_pass_debug_print(vulkan_render_pass *renderPass) {
  log_debug("renderPass:\n");
  vulkan_render_pass_shader_program_debug_print(renderPass->shaderProgram, 2);
  vulkan_render_pass_state_debug_print(renderPass->renderPassState, 2);
}

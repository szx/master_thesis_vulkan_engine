#include "render_pass.h"

size_t get_cache_len(vulkan_render_pass *renderPass) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(renderPass->vks->swapChainImages);
  return frameInFlightCount * swapChainImageCount;
}

size_t get_cache_index(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                       size_t swapChainImageIdx) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(renderPass->vks->swapChainImages);
  if (frameInFlightCount < swapChainImageCount) {
    return swapChainImageIdx * frameInFlightCount + currentFrameInFlight;
  }
  if (frameInFlightCount >= swapChainImageCount) {
    return currentFrameInFlight * swapChainImageCount + swapChainImageIdx;
  }
  UNREACHABLE;
}

VkRenderPass get_render_pass(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                             size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass *_renderPass = utarray_eltptr(renderPass->_renderPasses, cacheIndex);
  if (*_renderPass != VK_NULL_HANDLE) {
    return *_renderPass;
  }

  vulkan_render_pass_info renderPassInfo = vulkan_render_pass_get_renderPass_info(renderPass);
  bool useOnscreenColorAttachment = renderPassInfo.useOnscreenColorAttachment;
  uint32_t offscreenColorAttachmentCount =
      vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo);
  bool useDepthAttachment = renderPassInfo.useDepthAttachment;
  uint32_t dependencyCount = vulkan_render_pass_info_get_dependency_count(renderPassInfo);
  assert(useOnscreenColorAttachment || offscreenColorAttachmentCount > 0);

  vulkan_render_pass_frame_state *frameState =
      utarray_eltptr(renderPass->renderPassState->frameStates, currentFrameInFlight);

  VkFormat swapChainImageFormat = renderPass->vks->swapChainImageFormat;
  VkFormat offscreenFormats[VLA(offscreenColorAttachmentCount)];
  vulkan_render_pass_info_get_framebuffer_offscreen_attachment_formats(renderPassInfo, frameState,
                                                                       offscreenFormats);
  VkFormat depthBufferImageFormat =
      vulkan_render_pass_info_get_framebuffer_depth_attachment_formats(renderPassInfo, frameState);

  VkAttachmentDescription onscreenColorAttachmentDescription;
  VkAttachmentReference onscreenColorAttachmentReference;
  VkAttachmentDescription offscreenColorAttachmentDescriptions[VLA(offscreenColorAttachmentCount)];
  VkAttachmentReference offscreenColorAttachmentReferences[VLA(offscreenColorAttachmentCount)];
  VkAttachmentDescription depthAttachmentDescription;
  VkAttachmentReference depthAttachmentReference;
  VkSubpassDependency dependencies[VLA(dependencyCount)];

  vulkan_render_pass_info_get_render_pass_create_info(
      renderPassInfo, renderPass->prev, renderPass->next, swapChainImageFormat, offscreenFormats,
      depthBufferImageFormat, &onscreenColorAttachmentDescription,
      &onscreenColorAttachmentReference, offscreenColorAttachmentDescriptions,
      offscreenColorAttachmentReferences, &depthAttachmentDescription, &depthAttachmentReference,
      dependencies);

  *_renderPass = vulkan_create_render_pass(
      renderPass->vks->vkd,
      (useOnscreenColorAttachment ? &onscreenColorAttachmentDescription : NULL),
      (useOnscreenColorAttachment ? &onscreenColorAttachmentReference : NULL),
      offscreenColorAttachmentDescriptions, offscreenColorAttachmentCount,
      offscreenColorAttachmentReferences, offscreenColorAttachmentCount,
      (useDepthAttachment ? &depthAttachmentDescription : NULL),
      (useDepthAttachment ? &depthAttachmentReference : NULL), dependencies, dependencyCount,
      "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_render_pass_type_debug_str(renderPass->type), currentFrameInFlight, swapChainImageIdx);

  return *_renderPass;
}

VkFramebuffer get_framebuffer(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                              size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkFramebuffer *framebuffer = utarray_eltptr(renderPass->_framebuffers, cacheIndex);
  if (*framebuffer != VK_NULL_HANDLE) {
    return *framebuffer;
  }

  vulkan_render_pass_info renderPassInfo = vulkan_render_pass_get_renderPass_info(renderPass);

  uint32_t offscreenFramebufferAttachmentCount =
      vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo);

  vulkan_render_pass_frame_state *frameState =
      utarray_eltptr(renderPass->renderPassState->frameStates, currentFrameInFlight);

  VkImageView swapChainImageView =
      *(VkImageView *)utarray_eltptr(renderPass->vks->swapChainImageViews, swapChainImageIdx);
  VkImageView depthBufferImageView =
      vulkan_render_pass_info_get_framebuffer_depth_attachment_image_view(renderPassInfo,
                                                                          frameState);
  VkImageView offscreenImageViews[VLA(offscreenFramebufferAttachmentCount)];
  vulkan_render_pass_info_get_framebuffer_offscreen_attachment_image_views(
      renderPassInfo, frameState, offscreenImageViews);

  VkRenderPass _renderPass = get_render_pass(renderPass, currentFrameInFlight, swapChainImageIdx);

  uint32_t framebufferAttachmentCount =
      vulkan_render_pass_info_get_framebuffer_attachment_count(renderPassInfo);
  assert(framebufferAttachmentCount > 0);
  VkImageView framebufferAttachments[framebufferAttachmentCount];
  vulkan_render_pass_info_get_framebuffer_attachment_image_views(
      renderPassInfo, swapChainImageView, offscreenImageViews, depthBufferImageView,
      framebufferAttachments);

  *framebuffer = vulkan_create_framebuffer(
      renderPass->vks->vkd, _renderPass, framebufferAttachmentCount, framebufferAttachments,
      renderPass->vks->swapChainExtent.width, renderPass->vks->swapChainExtent.height,
      "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_render_pass_type_debug_str(renderPass->type), currentFrameInFlight, swapChainImageIdx);

  return *framebuffer;
}

VkPipeline get_graphics_renderPass(vulkan_render_pass *renderPass, size_t currentFrameInFlight,
                                   size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkPipeline *graphicsPipeline = utarray_eltptr(renderPass->_graphicsPipelines, cacheIndex);
  if (*graphicsPipeline != VK_NULL_HANDLE) {
    return *graphicsPipeline;
  }

  vulkan_render_pass_info renderPassInfo = vulkan_render_pass_get_renderPass_info(renderPass);
  uint32_t colorAttachmentCount =
      vulkan_render_pass_info_get_framebuffer_color_attachment_count(renderPassInfo);
  bool colorBlendingType = renderPassInfo.colorBlendingType;

  bool depthWriteEnable =
      renderPassInfo.useDepthAttachment && renderPassInfo.depthAttachmentWriteEnable;
  bool depthTestEnable =
      renderPassInfo.useDepthAttachment && renderPassInfo.depthAttachmentTestEnable;
  VkCompareOp depthTestOp = renderPassInfo.depthAttachmentTestOp;

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

  VkRenderPass _renderPass = get_render_pass(renderPass, currentFrameInFlight, swapChainImageIdx);

  *graphicsPipeline = vulkan_create_graphics_pipeline(
      renderPass->vks->vkd,

      colorAttachmentCount, colorBlendingType,

      depthWriteEnable, depthTestEnable, depthTestOp,

      shaderStages, shaderStageCount,

      &vertexInputBindingDescription, vertexBindingDescriptionsCount, vertexAttributeDescriptions,
      vertexAttributeDescriptionsCount,

      renderPass->vks->swapChainExtent.width, renderPass->vks->swapChainExtent.height,

      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      _renderPass, renderPass->renderState->descriptors->pipelineLayout,
      "renderPass %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_render_pass_type_debug_str(renderPass->type), currentFrameInFlight, swapChainImageIdx);

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);

  return *graphicsPipeline;
}

vulkan_render_pass *vulkan_render_pass_create_start(vulkan_render_pass_type type,
                                                    vulkan_swap_chain *vks,
                                                    vulkan_render_state *renderState,
                                                    vulkan_render_pass_state *renderPassState) {
  vulkan_render_pass *renderPass = core_alloc(sizeof(vulkan_render_pass));
  vulkan_render_pass_init_start(renderPass, type, vks, renderState, renderPassState);
  return renderPass;
}

void vulkan_render_pass_destroy(vulkan_render_pass *renderPass) {
  vulkan_render_pass_deinit(renderPass);
  core_free(renderPass);
}

void vulkan_render_pass_init_start(vulkan_render_pass *renderPass, vulkan_render_pass_type type,
                                   vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                   vulkan_render_pass_state *renderPassState) {
  renderPass->vks = vks;
  renderPass->renderState = renderState;
  renderPass->renderPassState = renderPassState;

  renderPass->type = type;
  renderPass->shaderProgram = vulkan_render_pass_shader_program_create(
      renderState, vulkan_render_pass_get_renderPass_info(renderPass));
}

void vulkan_render_pass_init_prev_next(vulkan_render_pass *renderPass, vulkan_render_pass *prev,
                                       vulkan_render_pass *next) {
  renderPass->prev = prev;
  renderPass->next = next;
}

void vulkan_render_pass_init_finish(vulkan_render_pass *renderPass) {
  utarray_alloc(renderPass->_renderPasses, sizeof(VkRenderPass));
  utarray_resize(renderPass->_renderPasses, get_cache_len(renderPass));
  utarray_foreach_elem_it (VkRenderPass *, renderPass, renderPass->_renderPasses) {
    *renderPass = VK_NULL_HANDLE;
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
    vkDestroyPipeline(renderPass->vks->vkd->device, *graphicsPipeline, vka);
  }
  utarray_foreach_elem_it (VkRenderPass *, _renderPass, renderPass->_renderPasses) {
    vkDestroyRenderPass(renderPass->vks->vkd->device, *_renderPass, vka);
  }
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, renderPass->_framebuffers) {
    vkDestroyFramebuffer(renderPass->vks->vkd->device, *framebuffer, vka);
  }
  vulkan_render_pass_shader_program_destroy(renderPass->shaderProgram);
}

vulkan_render_pass_info vulkan_render_pass_get_renderPass_info(vulkan_render_pass *renderPass) {
#define x(_name, ...)                                                                              \
  if (renderPass->type == vulkan_render_pass_type_##_name) {                                       \
    vulkan_render_pass_info renderPassInfo =                                                       \
        vulkan_render_pass_impl_##_name##_get_renderPass_info(renderPass);                         \
    renderPassInfo.renderPassType = renderPass->type;                                              \
    return renderPassInfo;                                                                         \
  }
  VULKAN_RENDER_PASS_TYPES(x, )
#undef x
  UNREACHABLE;
}

void vulkan_render_pass_record_commands(vulkan_render_pass *renderPass,
                                        VkCommandBuffer commandBuffer, size_t swapChainImageIdx) {
  size_t currentFrameInFlight = renderPass->renderState->sync->currentFrameInFlight;
  vulkan_render_pass_info renderPassInfo = vulkan_render_pass_get_renderPass_info(renderPass);

  VkFramebuffer framebuffer = get_framebuffer(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass _renderPass = get_render_pass(renderPass, currentFrameInFlight, swapChainImageIdx);
  VkPipeline graphicsPipeline =
      get_graphics_renderPass(renderPass, currentFrameInFlight, swapChainImageIdx);

  vulkan_render_pass_frame_state *frameState =
      utarray_eltptr(renderPass->renderPassState->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  // HIRO pass render pass and framebuffer as argument, allocate if NULL
  VkRenderPassBeginInfo renderPassBeginInfo = {0};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = _renderPass;
  renderPassBeginInfo.framebuffer = framebuffer;
  renderPassBeginInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassBeginInfo.renderArea.extent = renderPass->vks->swapChainExtent;
  uint32_t clearValueCount =
      vulkan_render_pass_info_get_framebuffer_attachment_count(renderPassInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_render_pass_info_get_framebuffer_attachment_clear_values(renderPassInfo, clearValues);
  renderPassBeginInfo.clearValueCount = array_size(clearValues);
  renderPassBeginInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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

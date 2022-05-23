#include "pipeline.h"

size_t get_cache_len(vulkan_pipeline *pipeline) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(pipeline->vks->swapChainImages);
  return frameInFlightCount * swapChainImageCount;
}

size_t get_cache_index(vulkan_pipeline *pipeline, size_t currentFrameInFlight,
                       size_t swapChainImageIdx) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(pipeline->vks->swapChainImages);
  if (frameInFlightCount < swapChainImageCount) {
    return swapChainImageIdx * frameInFlightCount + currentFrameInFlight;
  }
  if (frameInFlightCount >= swapChainImageCount) {
    return currentFrameInFlight * swapChainImageCount + swapChainImageIdx;
  }
  UNREACHABLE;
}

VkRenderPass get_render_pass(vulkan_pipeline *pipeline, size_t currentFrameInFlight,
                             size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(pipeline, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass *renderPass = utarray_eltptr(pipeline->_renderPasses, cacheIndex);
  if (*renderPass != VK_NULL_HANDLE) {
    return *renderPass;
  }

  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  bool useOnscreenColorAttachment = pipelineInfo.useOnscreenColorAttachment;
  uint32_t offscreenColorAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_offscreen_color_attachment_count(pipelineInfo);
  bool useDepthAttachment = pipelineInfo.useDepthAttachment;
  uint32_t dependencyCount = vulkan_pipeline_info_get_dependency_count(pipelineInfo);
  assert(useOnscreenColorAttachment || offscreenColorAttachmentCount > 0);

  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->pipelineState->frameStates, currentFrameInFlight);

  VkFormat swapChainImageFormat = pipeline->vks->swapChainImageFormat;
  VkFormat offscreenFormats[VLA(offscreenColorAttachmentCount)];
  vulkan_pipeline_info_get_framebuffer_offscreen_attachment_formats(pipelineInfo, frameState,
                                                                    offscreenFormats);
  VkFormat depthBufferImageFormat =
      vulkan_pipeline_info_get_framebuffer_depth_attachment_formats(pipelineInfo, frameState);

  VkAttachmentDescription onscreenColorAttachmentDescription;
  VkAttachmentReference onscreenColorAttachmentReference;
  VkAttachmentDescription offscreenColorAttachmentDescriptions[VLA(offscreenColorAttachmentCount)];
  VkAttachmentReference offscreenColorAttachmentReferences[VLA(offscreenColorAttachmentCount)];
  VkAttachmentDescription depthAttachmentDescription;
  VkAttachmentReference depthAttachmentReference;
  VkSubpassDependency dependencies[VLA(dependencyCount)];

  vulkan_pipeline_info_get_render_pass_create_info(
      pipelineInfo, pipeline->prev, pipeline->next, swapChainImageFormat, offscreenFormats,
      depthBufferImageFormat, &onscreenColorAttachmentDescription,
      &onscreenColorAttachmentReference, offscreenColorAttachmentDescriptions,
      offscreenColorAttachmentReferences, &depthAttachmentDescription, &depthAttachmentReference,
      dependencies);

  *renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, (useOnscreenColorAttachment ? &onscreenColorAttachmentDescription : NULL),
      (useOnscreenColorAttachment ? &onscreenColorAttachmentReference : NULL),
      offscreenColorAttachmentDescriptions, offscreenColorAttachmentCount,
      offscreenColorAttachmentReferences, offscreenColorAttachmentCount,
      (useDepthAttachment ? &depthAttachmentDescription : NULL),
      (useDepthAttachment ? &depthAttachmentReference : NULL), dependencies, dependencyCount,
      "pipeline %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_pipeline_type_debug_str(pipeline->type), currentFrameInFlight, swapChainImageIdx);

  return *renderPass;
}

VkFramebuffer get_framebuffer(vulkan_pipeline *pipeline, size_t currentFrameInFlight,
                              size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(pipeline, currentFrameInFlight, swapChainImageIdx);
  VkFramebuffer *framebuffer = utarray_eltptr(pipeline->_framebuffers, cacheIndex);
  if (*framebuffer != VK_NULL_HANDLE) {
    return *framebuffer;
  }

  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);

  uint32_t offscreenFramebufferAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_offscreen_color_attachment_count(pipelineInfo);

  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->pipelineState->frameStates, currentFrameInFlight);

  VkImageView swapChainImageView =
      *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx);
  VkImageView depthBufferImageView =
      vulkan_pipeline_info_get_framebuffer_depth_attachment_image_view(pipelineInfo, frameState);
  VkImageView offscreenImageViews[VLA(offscreenFramebufferAttachmentCount)];
  vulkan_pipeline_info_get_framebuffer_offscreen_attachment_image_views(pipelineInfo, frameState,
                                                                        offscreenImageViews);

  VkRenderPass renderPass = get_render_pass(pipeline, currentFrameInFlight, swapChainImageIdx);

  uint32_t framebufferAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  assert(framebufferAttachmentCount > 0);
  VkImageView framebufferAttachments[framebufferAttachmentCount];
  vulkan_pipeline_info_get_framebuffer_attachment_image_views(
      pipelineInfo, swapChainImageView, offscreenImageViews, depthBufferImageView,
      framebufferAttachments);

  *framebuffer = vulkan_create_framebuffer(
      pipeline->vks->vkd, renderPass, framebufferAttachmentCount, framebufferAttachments,
      pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,
      "pipeline %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_pipeline_type_debug_str(pipeline->type), currentFrameInFlight, swapChainImageIdx);

  return *framebuffer;
}

VkPipeline get_graphics_pipeline(vulkan_pipeline *pipeline, size_t currentFrameInFlight,
                                 size_t swapChainImageIdx) {
  size_t cacheIndex = get_cache_index(pipeline, currentFrameInFlight, swapChainImageIdx);
  VkPipeline *graphicsPipeline = utarray_eltptr(pipeline->_graphicsPipelines, cacheIndex);
  if (*graphicsPipeline != VK_NULL_HANDLE) {
    return *graphicsPipeline;
  }

  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  uint32_t colorAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_color_attachment_count(pipelineInfo);
  bool colorBlendingType = pipelineInfo.colorBlendingType;

  bool depthWriteEnable =
      pipelineInfo.useDepthAttachment && pipelineInfo.depthAttachmentWriteEnable;
  bool depthTestEnable = pipelineInfo.useDepthAttachment && pipelineInfo.depthAttachmentTestEnable;
  VkCompareOp depthTestOp = pipelineInfo.depthAttachmentTestOp;

  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_pipeline_shader_program_get_shader_stages(pipeline->shaderProgram, &shaderStageCount);

  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  uint32_t vertexBindingDescriptionsCount =
      vulkan_vertex_stream_get_vertex_buffer_binding_count(pipeline->renderState->vertexStream);
  assert(vertexBindingDescriptionsCount == 1);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_vertex_stream_get_vertex_buffer_binding_description(
          pipeline->renderState->vertexStream);

  uint32_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_vertex_stream_get_vertex_attribute_descriptions(pipeline->renderState->vertexStream,
                                                             &vertexAttributeDescriptionsCount);

  VkRenderPass renderPass = get_render_pass(pipeline, currentFrameInFlight, swapChainImageIdx);

  *graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd,

      colorAttachmentCount, colorBlendingType,

      depthWriteEnable, depthTestEnable, depthTestOp,

      shaderStages, shaderStageCount,

      &vertexInputBindingDescription, vertexBindingDescriptionsCount, vertexAttributeDescriptions,
      vertexAttributeDescriptionsCount,

      pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,

      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      renderPass, pipeline->renderState->descriptors->pipelineLayout,
      "pipeline %s (frameInFlight=#%u, swapChainImage=#%u)",
      vulkan_pipeline_type_debug_str(pipeline->type), currentFrameInFlight, swapChainImageIdx);

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);

  return *graphicsPipeline;
}

vulkan_pipeline *vulkan_pipeline_create_start(vulkan_pipeline_type type, vulkan_swap_chain *vks,
                                              vulkan_render_state *renderState,
                                              vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));
  vulkan_pipeline_init_start(pipeline, type, vks, renderState, pipelineState);
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_pipeline_deinit(pipeline);
  core_free(pipeline);
}

void vulkan_pipeline_init_start(vulkan_pipeline *pipeline, vulkan_pipeline_type type,
                                vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                vulkan_pipeline_state *pipelineState) {
  pipeline->vks = vks;
  pipeline->renderState = renderState;
  pipeline->pipelineState = pipelineState;

  pipeline->type = type;
  pipeline->shaderProgram = vulkan_pipeline_shader_program_create(
      renderState, vulkan_pipeline_get_pipeline_info(pipeline));
}

void vulkan_pipeline_init_prev_next(vulkan_pipeline *pipeline, vulkan_pipeline *prev,
                                    vulkan_pipeline *next) {
  pipeline->prev = prev;
  pipeline->next = next;
}

void vulkan_pipeline_init_finish(vulkan_pipeline *pipeline) {
  utarray_alloc(pipeline->_renderPasses, sizeof(VkRenderPass));
  utarray_resize(pipeline->_renderPasses, get_cache_len(pipeline));
  utarray_foreach_elem_it (VkRenderPass *, renderPass, pipeline->_renderPasses) {
    *renderPass = VK_NULL_HANDLE;
  }

  utarray_alloc(pipeline->_framebuffers, sizeof(VkFramebuffer));
  utarray_resize(pipeline->_framebuffers, get_cache_len(pipeline));
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, pipeline->_framebuffers) {
    *framebuffer = VK_NULL_HANDLE;
  }

  utarray_alloc(pipeline->_graphicsPipelines, sizeof(VkPipeline));
  utarray_resize(pipeline->_graphicsPipelines, get_cache_len(pipeline));
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, pipeline->_graphicsPipelines) {
    *graphicsPipeline = VK_NULL_HANDLE;
  }
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, pipeline->_graphicsPipelines) {
    vkDestroyPipeline(pipeline->vks->vkd->device, *graphicsPipeline, vka);
  }
  utarray_foreach_elem_it (VkRenderPass *, renderPass, pipeline->_renderPasses) {
    vkDestroyRenderPass(pipeline->vks->vkd->device, *renderPass, vka);
  }
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, pipeline->_framebuffers) {
    vkDestroyFramebuffer(pipeline->vks->vkd->device, *framebuffer, vka);
  }
  vulkan_pipeline_shader_program_destroy(pipeline->shaderProgram);
}

vulkan_pipeline_info vulkan_pipeline_get_pipeline_info(vulkan_pipeline *pipeline) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_info pipelineInfo =                                                            \
        vulkan_pipeline_impl_##_name##_get_pipeline_info(pipeline);                                \
    pipelineInfo.pipelineType = pipeline->type;                                                    \
    return pipelineInfo;                                                                           \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
  UNREACHABLE;
}

void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                        size_t swapChainImageIdx) {
  size_t currentFrameInFlight = pipeline->renderState->sync->currentFrameInFlight;
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);

  VkFramebuffer framebuffer = get_framebuffer(pipeline, currentFrameInFlight, swapChainImageIdx);
  VkRenderPass renderPass = get_render_pass(pipeline, currentFrameInFlight, swapChainImageIdx);
  VkPipeline graphicsPipeline =
      get_graphics_pipeline(pipeline, currentFrameInFlight, swapChainImageIdx);

  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->pipelineState->frameStates, currentFrameInFlight);

  /* record new render pass into command buffer */
  // HIRO pass render pass and framebuffer as argument, allocate if NULL
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = pipeline->vks->swapChainExtent;
  uint32_t clearValueCount = vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  VkClearValue clearValues[clearValueCount];
  vulkan_pipeline_info_get_framebuffer_attachment_clear_values(pipelineInfo, clearValues);
  renderPassInfo.clearValueCount = array_size(clearValues);
  renderPassInfo.pClearValues = clearValues;
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_record_render_pass(pipeline, frameState, commandBuffer);        \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x

  vkCmdEndRenderPass(commandBuffer);
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("pipeline:\n");
  vulkan_pipeline_shader_program_debug_print(pipeline->shaderProgram, 2);
  vulkan_pipeline_state_debug_print(pipeline->pipelineState, 2);
}

#include "render_pass.h"

size_t get_cache_len(render_pass *renderPass) {
  size_t frameInFlightCount = FRAMES_IN_FLIGHT;
  size_t swapChainImageCount = utarray_len(renderPass->renderState->vks->swapChainImages);
  return frameInFlightCount * swapChainImageCount;
}

size_t get_cache_index(render_pass *renderPass, size_t currentFrameInFlight,
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

VkPipeline get_graphics_pipeline(render_pass *renderPass, size_t currentFrameInFlight,
                                 size_t swapChainImageIdx, rendering_info renderingInfo) {
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
      render_pass_shader_program_get_shader_stages(renderPass->shaderProgram, &shaderStageCount);

  uint32_t vertexBindingDescriptionsCount =
      vertex_stream_get_vertex_buffer_binding_count(renderPass->renderState->vertexStream);
  assert(vertexBindingDescriptionsCount == 1);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vertex_stream_get_vertex_buffer_binding_description(renderPass->renderState->vertexStream);

  uint32_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vertex_stream_get_vertex_attribute_descriptions(renderPass->renderState->vertexStream,
                                                      &vertexAttributeDescriptionsCount);

  graphics_pipeline_create_info createInfo = {
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

      .renderPass = NULL,
      .pipelineLayout = renderPass->renderState->descriptors->pipelineLayout,
  };

  *graphicsPipeline =
      create_graphics_pipeline(renderPass->renderState->vkd, createInfo, renderingInfo,
                               "renderPass (frameInFlight=#%u, swapChainImage=#%u)",
                               currentFrameInFlight, swapChainImageIdx);

  core_free(vertexAttributeDescriptions);
  core_free(shaderStages);

  return *graphicsPipeline;
}

render_pass *render_pass_create(render_pass_desc desc, render_state *renderState,
                                render_pass_state *renderPassState) {
  render_pass *renderPass = core_alloc(sizeof(render_pass));
  render_pass_init(renderPass, desc, renderState, renderPassState);
  return renderPass;
}

void render_pass_destroy(render_pass *renderPass) {
  render_pass_deinit(renderPass);
  core_free(renderPass);
}

void render_pass_init(render_pass *renderPass, render_pass_desc desc, render_state *renderState,
                      render_pass_state *renderPassState) {
  renderPass->renderState = renderState;
  renderPass->renderPassState = renderPassState;

  renderPass->desc = desc;
  renderPass->shaderProgram = render_pass_shader_program_create(renderState, renderPass->desc);

  /* cache */
  utarray_alloc(renderPass->_graphicsPipelines, sizeof(VkPipeline));
  utarray_resize(renderPass->_graphicsPipelines, get_cache_len(renderPass));
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, renderPass->_graphicsPipelines) {
    *graphicsPipeline = VK_NULL_HANDLE;
  }
}

void render_pass_deinit(render_pass *renderPass) {
  utarray_foreach_elem_it (VkPipeline *, graphicsPipeline, renderPass->_graphicsPipelines) {
    vkDestroyPipeline(renderPass->renderState->vkd->device, *graphicsPipeline, vka);
  }
  utarray_free(renderPass->_graphicsPipelines);
  render_pass_shader_program_destroy(renderPass->shaderProgram);
}

void set_rendering_info_images(render_pass_frame_state *frameState, size_t swapChainImageIdx,
                               rendering_info *renderPassInfo) {
  if (renderPassInfo->onscreenColorAttachment) {
    VkImageView swapChainImageView = *(VkImageView *)utarray_eltptr(
        frameState->renderPassState->renderState->vks->swapChainImageViews, swapChainImageIdx);
    renderPassInfo->onscreenColorAttachment->imageView = swapChainImageView;
  }
  for (size_t i = 0; i < utarray_len(renderPassInfo->offscreenColorAttachments); i++) {
    rendering_attachment_info *attachmentInfo =
        utarray_eltptr(renderPassInfo->offscreenColorAttachments, i);
    image *image = render_pass_offscreen_texture_state_get_offscreen_texture(
                       frameState->offscreenTextures, attachmentInfo->name)
                       ->image;
    attachmentInfo->imageView = image->imageView;
  }
  if (renderPassInfo->depthAttachment) {
    image *image = render_pass_offscreen_texture_state_get_offscreen_texture(
                       frameState->offscreenTextures, renderPassInfo->depthAttachment->name)
                       ->image;
    renderPassInfo->depthAttachment->imageView = image->imageView;
  }

  utarray_foreach_elem_it (rendering_image_layout_transition_info *, imageLayoutTransitionInfo,
                           renderPassInfo->preImageLayoutTransition) {
    if (strcmp(imageLayoutTransitionInfo->name, "swap chain") == 0) {
      VkImage swapChainImage = *(VkImage *)utarray_eltptr(
          frameState->renderPassState->renderState->vks->swapChainImages, swapChainImageIdx);
      imageLayoutTransitionInfo->image = swapChainImage;
      imageLayoutTransitionInfo->imageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    } else {
      image *image = render_pass_offscreen_texture_state_get_offscreen_texture(
                         frameState->offscreenTextures, imageLayoutTransitionInfo->name)
                         ->image;
      imageLayoutTransitionInfo->image = image->image;
      imageLayoutTransitionInfo->imageAspectFlags = image->aspectFlags;
    }
  }

  utarray_foreach_elem_it (rendering_image_layout_transition_info *, imageLayoutTransitionInfo,
                           renderPassInfo->postImageLayoutTransition) {
    if (strcmp(imageLayoutTransitionInfo->name, "swap chain") == 0) {
      VkImage swapChainImage = *(VkImage *)utarray_eltptr(
          frameState->renderPassState->renderState->vks->swapChainImages, swapChainImageIdx);
      imageLayoutTransitionInfo->image = swapChainImage;
      imageLayoutTransitionInfo->imageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    } else {
      image *image = render_pass_offscreen_texture_state_get_offscreen_texture(
                         frameState->offscreenTextures, imageLayoutTransitionInfo->name)
                         ->image;
      imageLayoutTransitionInfo->image = image->image;
      imageLayoutTransitionInfo->imageAspectFlags = image->aspectFlags;
    }
  }
}

void render_pass_record_commands(render_pass *renderPass, VkCommandBuffer commandBuffer,
                                 size_t swapChainImageIdx, rendering_info renderPassInfo) {
  size_t currentFrameInFlight = renderPass->renderState->sync->currentFrameInFlight;
  render_pass_frame_state *frameState =
      render_pass_state_get_frame_state(renderPass->renderPassState, currentFrameInFlight);

  set_rendering_info_images(frameState, swapChainImageIdx, &renderPassInfo);

  VkPipeline _graphicsPipeline =
      get_graphics_pipeline(renderPass, currentFrameInFlight, swapChainImageIdx, renderPassInfo);

  /* record new render pass into command buffer */
  begin_rendering(renderPass->renderState->vkd, commandBuffer, renderPassInfo);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

  renderPass->desc.recordFunc(renderPass, frameState, commandBuffer);

  end_rendering(renderPass->renderState->vkd, commandBuffer, renderPassInfo);
}

void render_pass_debug_print(render_pass *renderPass) {
  log_debug("renderPass:\n");
  render_pass_shader_program_debug_print(renderPass->shaderProgram, 2);
  render_pass_state_debug_print(renderPass->renderPassState, 2);
}

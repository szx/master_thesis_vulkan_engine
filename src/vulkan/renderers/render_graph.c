#include "render_graph.h"

/* render graph resource element */

void vulkan_image_render_pass_usage_timeline_add_new_usage(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    vulkan_image_render_pass_usage usage) {
  usageTimeline->usages[renderGraphIdx] |= usage;
}

void vulkan_image_render_pass_usage_timeline_add_new_format(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkFormat format) {
  usageTimeline->formats[renderGraphIdx] = format;
}

void vulkan_image_render_pass_usage_timeline_add_new_clear_value(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkClearValue clearValue) {
  usageTimeline->clearValues[renderGraphIdx] = clearValue;
}

const char *usage_to_str(vulkan_image_render_pass_usage usage) {
  if (usage == vulkan_image_render_pass_usage_read) {
    return "R";
  }
  if (usage == vulkan_image_render_pass_usage_write) {
    return "W";
  }
  if (usage == vulkan_image_render_pass_usage_read_write) {
    return "RW";
  }
  return "_";
}

void vulkan_image_render_pass_usage_timeline_debug_print(
    vulkan_image_render_pass_usage_timeline *usageTimeline, int indent) {
  log_debug(INDENT_FORMAT_STRING "usage timeline:", INDENT_FORMAT_ARGS(0));
  for (size_t i = 0; i < MAX_RENDER_PASS_COUNT; i++) {
    vulkan_image_render_pass_usage_timeline_info info =
        vulkan_image_render_pass_usage_timeline_get_info(usageTimeline, i);
    log_debug(INDENT_FORMAT_STRING "%zu:\t%s\t%s\t%s ", INDENT_FORMAT_ARGS(2), i,
              usage_to_str(info.previousUsage), usage_to_str(info.currentUsage),
              usage_to_str(info.nextUsage));
  }
}
vulkan_image_render_pass_usage_timeline_info vulkan_image_render_pass_usage_timeline_get_info(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx) {
  vulkan_image_render_pass_usage_timeline_info info = {0};

  bool foundUsage = false;
  bool foundFormat = false;
  if (renderGraphIdx > 0) {
    for (size_t i = renderGraphIdx - 1;; i--) {
      if (usageTimeline->usages[i] && !foundUsage) {
        info.previousUsage = usageTimeline->usages[i];
        foundUsage = true;
      }
      if (usageTimeline->formats[i] && !foundFormat) {
        info.previousFormat = usageTimeline->formats[i];
        foundFormat = true;
      }
      if (i == 0 || (foundUsage && foundFormat)) {
        break;
      }
    }
  }

  if (usageTimeline->usages[renderGraphIdx]) {
    info.currentUsage = usageTimeline->usages[renderGraphIdx];
  }
  if (usageTimeline->formats[renderGraphIdx]) {
    info.currentFormat = usageTimeline->formats[renderGraphIdx];
  }
  info.clearValue = usageTimeline->clearValues[renderGraphIdx];

  foundUsage = false;
  foundFormat = false;
  for (size_t i = renderGraphIdx + 1; i < MAX_RENDER_PASS_COUNT; i++) {
    if (usageTimeline->usages[i] && !foundUsage) {
      info.nextUsage = usageTimeline->usages[i];
      foundUsage = true;
    }
    if (usageTimeline->formats[i] && !foundFormat) {
      info.nextFormat = usageTimeline->formats[i];
      foundFormat = true;
    }
    if (foundUsage && foundFormat) {
      break;
    }
  }

  return info;
}

void vulkan_render_graph_resource_init(vulkan_render_graph_resource *element, const char *name,
                                       vulkan_image_type imageType) {
  element->name = core_strdup(name);
  element->imageType = imageType;
  element->offscreenTextureIdx = MAX_OFFSCREEN_TEXTURE_COUNT;
  element->usageTimeline = (struct vulkan_image_render_pass_usage_timeline){0};
  element->prev = NULL;
  element->next = NULL;
}

void vulkan_render_graph_resource_deinit(vulkan_render_graph_resource *element) {
  core_free((char *)element->name);
}

void vulkan_render_graph_resource_debug_print(vulkan_render_graph_resource *element) {
  log_raw(stdout, "\"render graph resource\\n%p\\n", element);
  log_raw(stdout, "state: \\n");
  log_raw(stdout, "\tname: %s\\n", element->name);
  log_raw(stdout, "\timageType: %s\\n", vulkan_image_type_debug_str(element->imageType));
  log_raw(stdout, "\" ");
}

/* render graph render pass element */

vulkan_render_graph_render_pass_element *
vulkan_render_graph_render_pass_element_create(size_t renderPassIdx, vulkan_render_pass *renderPass,
                                               vulkan_render_graph *renderGraph) {
  vulkan_render_graph_render_pass_element *element =
      core_alloc(sizeof(vulkan_render_graph_render_pass_element));

  element->idx = renderPassIdx;
  element->renderPass = renderPass;
  utarray_alloc(element->offscreenTextures, sizeof(vulkan_render_graph_resource *));
  element->swapChainImageResource = NULL;
  element->depthBufferResource = NULL;

  /* add resources from render pass info */
  for (size_t i = 0; i < renderPass->desc.offscreenFragmentShaderInputCount; i++) {
    vulkan_offscreen_fragment_shader_input_info *offscreenFragmentShaderInputInfo =
        &renderPass->desc.offscreenFragmentShaderInputs[i];

    vulkan_render_graph_resource *offscreenTextureResource =
        vulkan_render_graph_get_image_resource(renderGraph, offscreenFragmentShaderInputInfo->name);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    vulkan_image_render_pass_usage_timeline_add_new_usage(&offscreenTextureResource->usageTimeline,
                                                          element->idx,
                                                          vulkan_image_render_pass_usage_read);
    vulkan_image_render_pass_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType,
                                 0));
  }

  for (size_t i = 0; i < renderPass->desc.offscreenColorAttachmentCount; i++) {
    vulkan_offscreen_framebuffer_color_attachment_info *offscreenColorAttachmentInfo =
        &renderPass->desc.offscreenColorAttachments[i];

    vulkan_render_graph_resource *offscreenTextureResource =
        vulkan_render_graph_get_image_resource(renderGraph, offscreenColorAttachmentInfo->name);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    vulkan_image_render_pass_usage_timeline_add_new_usage(&offscreenTextureResource->usageTimeline,
                                                          element->idx,
                                                          vulkan_image_render_pass_usage_write);
    vulkan_image_render_pass_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType,
                                 0));
    vulkan_image_render_pass_usage_timeline_add_new_clear_value(
        &offscreenTextureResource->usageTimeline, element->idx,
        (VkClearValue){.color = offscreenColorAttachmentInfo->clearValue});
  }

  if (renderPass->desc.useOnscreenColorAttachment) {
    vulkan_render_graph_resource *swapChainImageResource =
        vulkan_render_graph_get_image_resource(renderGraph, "swap chain");
    element->swapChainImageResource = swapChainImageResource;

    vulkan_image_render_pass_usage_timeline_add_new_usage(
        &swapChainImageResource->usageTimeline, element->idx, vulkan_image_render_pass_usage_write);
    vulkan_image_render_pass_usage_timeline_add_new_format(
        &swapChainImageResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, swapChainImageResource->imageType,
                                 0));
    vulkan_image_render_pass_usage_timeline_add_new_clear_value(
        &swapChainImageResource->usageTimeline, element->idx,
        (VkClearValue){.color = renderPass->desc.onscreenClearValue});
  }

  if (renderPass->desc.useDepthAttachment) {
    vulkan_render_graph_resource *depthBufferResource =
        vulkan_render_graph_get_image_resource(renderGraph, "depth buffer");
    element->depthBufferResource = depthBufferResource;

    if (renderPass->desc.depthAttachmentWriteEnable) {
      vulkan_image_render_pass_usage_timeline_add_new_usage(
          &depthBufferResource->usageTimeline, element->idx, vulkan_image_render_pass_usage_write);
    }
    if (renderPass->desc.depthAttachmentTestEnable) {
      vulkan_image_render_pass_usage_timeline_add_new_usage(
          &depthBufferResource->usageTimeline, element->idx, vulkan_image_render_pass_usage_read);
    }
    vulkan_image_render_pass_usage_timeline_add_new_format(
        &depthBufferResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, depthBufferResource->imageType, 0));
    vulkan_image_render_pass_usage_timeline_add_new_clear_value(
        &depthBufferResource->usageTimeline, element->idx,
        (VkClearValue){.depthStencil = renderPass->desc.depthClearValue});
  }

  element->state = (struct vulkan_render_graph_render_pass_element_state){0};

  element->prev = NULL;
  element->next = NULL;

  return element;
}

void vulkan_render_graph_render_pass_element_destroy(
    vulkan_render_graph_render_pass_element *element) {
  vulkan_render_pass_destroy(element->renderPass);
  utarray_free(element->offscreenTextures);
  vulkan_render_pass_info_deinit(&element->state.renderPassInfo);
  core_free(element);
}

void vulkan_render_graph_render_pass_element_debug_print(
    vulkan_render_graph_render_pass_element *element) {
  log_raw(stdout, "\"render graph render pass element\\n%p\\n", element);
  log_raw(stdout, "idx: %zu\\n", element->idx);
  log_raw(stdout, "\" ");
}

/* render graph */

vulkan_render_graph *vulkan_render_graph_create(vulkan_render_state *renderState) {

  vulkan_render_graph *renderGraph = core_alloc(sizeof(vulkan_render_graph));

  renderGraph->renderState = renderState;

  renderGraph->renderPassState = vulkan_render_pass_state_create(renderGraph->renderState);

  renderGraph->renderPassElements = NULL;

  renderGraph->resources = NULL;

  renderGraph->_renderPassIdx = 0;
  renderGraph->_compiledResources = false;
  renderGraph->_compiledRenderPasses = false;

  vulkan_render_graph_add_image_resource(renderGraph, "swap chain", vulkan_image_type_swap_chain);
  vulkan_render_graph_add_image_resource(renderGraph, "depth buffer",
                                         vulkan_image_type_depth_buffer);

  return renderGraph;
}

void vulkan_render_graph_destroy(vulkan_render_graph *renderGraph) {

  dl_foreach_elem(vulkan_render_graph_resource *, element, renderGraph->resources) {
    vulkan_render_graph_resource_deinit(element);
    core_free(element);
  }

  dl_foreach_elem(vulkan_render_graph_render_pass_element *, element,
                  renderGraph->renderPassElements) {
    vulkan_render_graph_render_pass_element_destroy(element);
  }

  vulkan_render_pass_state_destroy(renderGraph->renderPassState);

  core_free(renderGraph);
}

void vulkan_render_graph_start_swap_chain_recreation(vulkan_render_graph *renderGraph) {

  dl_foreach_elem(vulkan_render_graph_render_pass_element *, element,
                  renderGraph->renderPassElements) {
    vulkan_render_pass_deinit(element->renderPass);
  }

  // vulkan_render_pass_state_deinit(renderer->renderPassState);
}

void vulkan_render_graph_finish_swap_chain_recreation(vulkan_render_graph *renderGraph) {

  // vulkan_render_pass_state_init(renderer->renderPassState, renderer->renderState);
  vulkan_render_pass_state_reinit_with_new_swap_chain(renderGraph->renderPassState);

  dl_foreach_elem(vulkan_render_graph_render_pass_element *, element,
                  renderGraph->renderPassElements) {
    vulkan_render_pass_init(element->renderPass, element->renderPass->desc,
                            element->renderPass->renderState, element->renderPass->renderPassState);
  }
}

/// Add offscreen images to render pass state.
void compile_offscreen_texture_resources(vulkan_render_graph *renderGraph) {
  dl_foreach_elem(vulkan_render_graph_resource *, resource, renderGraph->resources) {
    if (resource->imageType >= vulkan_image_type_offscreen_first &&
        resource->imageType <= vulkan_image_type_offscreen_count) {
      resource->offscreenTextureIdx = vulkan_render_pass_state_add_offscreen_texture(
          renderGraph->renderPassState, resource->name, resource->imageType);
    }
  }
}

/// Add offscreen images to render pass state.
void render_pass_desc_add_offscreen_texture_idxes(vulkan_render_graph *renderGraph,
                                                  vulkan_render_pass_desc *desc) {
  vulkan_render_pass_desc_calculate_additional_info(desc);

  dl_foreach_elem(vulkan_render_graph_resource *, resource, renderGraph->resources) {
    if (resource->imageType >= vulkan_image_type_offscreen_first &&
        resource->imageType <= vulkan_image_type_offscreen_count) {
      for (size_t i = 0; i < desc->offscreenFragmentShaderInputCount; i++) {
        if (strcmp(desc->offscreenFragmentShaderInputs[i].name, resource->name) == 0) {
          assert(resource->offscreenTextureIdx < MAX_OFFSCREEN_TEXTURE_COUNT);
          desc->offscreenFragmentShaderInputs[i]._offscreenTextureIdx =
              resource->offscreenTextureIdx;
          break;
        }
      }
    }
  }
}

void vulkan_render_graph_add_render_pass(vulkan_render_graph *renderGraph,
                                         vulkan_render_pass_desc desc) {
  assert(!renderGraph->_compiledRenderPasses);
  if (!renderGraph->_compiledResources) {
    compile_offscreen_texture_resources(renderGraph);
    renderGraph->_compiledResources = true;
  }
  assert(renderGraph->_renderPassIdx < MAX_RENDER_PASS_COUNT);

  render_pass_desc_add_offscreen_texture_idxes(renderGraph, &desc);

  vulkan_render_pass *renderPass =
      vulkan_render_pass_create(desc, renderGraph->renderState, renderGraph->renderPassState);

  vulkan_render_graph_render_pass_element *renderPassElement =
      vulkan_render_graph_render_pass_element_create(renderGraph->_renderPassIdx++, renderPass,
                                                     renderGraph);

  DL_APPEND(renderGraph->renderPassElements, renderPassElement);

  renderGraph->_compiledRenderPasses = false;
}

VkImageLayout get_image_layout_for_swap_chain_image(vulkan_image_render_pass_usage usage) {
  bool isWriteUsage = (usage & vulkan_image_render_pass_usage_write) != 0;
  assert((usage & vulkan_image_render_pass_usage_read) == 0);
  if (isWriteUsage) {
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_depth_buffer(vulkan_image_render_pass_usage usage) {
  bool isReadUsage = (usage & vulkan_image_render_pass_usage_read) != 0;
  bool isWriteUsage = (usage & vulkan_image_render_pass_usage_write) != 0;
  bool isReadWriteUsage = isReadUsage && isWriteUsage;
  if (isReadWriteUsage || isWriteUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
  if (isReadUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_offscreen_texture(vulkan_image_render_pass_usage usage) {
  bool isReadUsage = (usage & vulkan_image_render_pass_usage_read) != 0;
  bool isWriteUsage = (usage & vulkan_image_render_pass_usage_write) != 0;
  bool isReadWriteUsage = isReadUsage && isWriteUsage;
  if (isReadWriteUsage) {
    // NOTE: Currently VK_IMAGE_LAYOUT_GENERAL is unused.
    assert(0);
    return VK_IMAGE_LAYOUT_GENERAL;
  }
  if (isReadUsage) {
    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  if (isWriteUsage) {
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

/// Analyze render graph and fill in render pass create info.
void compile_render_pass(vulkan_render_graph_render_pass_element *renderPassElement) {
  bool swapChainImageResourceUsed = renderPassElement->swapChainImageResource != NULL;
  bool depthBufferResourceUsed = renderPassElement->depthBufferResource != NULL;

  struct vulkan_render_graph_render_pass_element_state *state = &renderPassElement->state;
  vulkan_render_pass_info_init(&state->renderPassInfo);

  if (swapChainImageResourceUsed) {
    vulkan_image_render_pass_usage_timeline_debug_print(
        &renderPassElement->swapChainImageResource->usageTimeline, 0);

    vulkan_image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_image_render_pass_usage_timeline_get_info(
            &renderPassElement->swapChainImageResource->usageTimeline, renderPassElement->idx);
    assert(resourceUsageTimelineInfo.currentUsage == vulkan_image_render_pass_usage_write);
    assert(resourceUsageTimelineInfo.currentFormat);

    // Image layout of swap chain image is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    vulkan_render_pass_attachment_create_info attachmentCreateInfo = {0};
    attachmentCreateInfo.format = resourceUsageTimelineInfo.currentFormat;
    if (resourceUsageTimelineInfo.previousUsage) {
      // Image layout either undefined or already transitioned at the end of previous render pass.
      attachmentCreateInfo.previousLayout =
          get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage);
    } else {
      attachmentCreateInfo.previousLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    attachmentCreateInfo.currentLayout =
        get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage);
    if (resourceUsageTimelineInfo.nextUsage) {
      attachmentCreateInfo.nextLayout =
          get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.nextUsage);
    } else {
      // Vulkan spec: A swapchain’s image must be transitioned to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      // layout before calling vkQueuePresentKHR.
      attachmentCreateInfo.nextLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
    vulkan_render_pass_info_add_onscreen_color_attachment(&state->renderPassInfo,
                                                          attachmentCreateInfo);

    // Make sure that finished previous rendering to onscreen texture before writing to color
    // attachment.
    vulkan_render_pass_info_add_execution_barrier(&state->renderPassInfo,
                                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    vulkan_render_pass_info_add_memory_barrier(&state->renderPassInfo, 0,
                                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
  }

  utarray_foreach_elem_deref (vulkan_render_graph_resource *, offscreenTextureResource,
                              renderPassElement->offscreenTextures) {
    vulkan_image_render_pass_usage_timeline_debug_print(&offscreenTextureResource->usageTimeline,
                                                        0);

    vulkan_image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_image_render_pass_usage_timeline_get_info(&offscreenTextureResource->usageTimeline,
                                                         renderPassElement->idx);

    bool isReadUsage =
        (resourceUsageTimelineInfo.currentUsage & vulkan_image_render_pass_usage_read) != 0;
    bool isWriteUsage =
        (resourceUsageTimelineInfo.currentUsage & vulkan_image_render_pass_usage_write) != 0;

    if (isReadUsage) {
      // Make sure that finished rendering to offscreen textures before sampling from them in
      // fragment shader.
      vulkan_render_pass_info_add_execution_barrier(&state->renderPassInfo,
                                                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                                                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      vulkan_render_pass_info_add_memory_barrier(&state->renderPassInfo, 0,
                                                 VK_ACCESS_SHADER_READ_BIT);
    }

    if (isWriteUsage) {

      // Image layout of offscreen texture is one of following:
      //  - VK_IMAGE_LAYOUT_UNDEFINED
      //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (if used as framebuffer color attachment)
      //  - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (if read from fragment shader)

      vulkan_render_pass_attachment_create_info attachmentCreateInfo = {0};
      attachmentCreateInfo.format = resourceUsageTimelineInfo.currentFormat;
      if (resourceUsageTimelineInfo.previousUsage) {
        // Image layout either undefined or already transitioned at the end of previous render pass.
        attachmentCreateInfo.previousLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      } else {
        attachmentCreateInfo.previousLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      }
      attachmentCreateInfo.currentLayout =
          get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      if (resourceUsageTimelineInfo.nextUsage) {
        attachmentCreateInfo.nextLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.nextUsage);
      } else {
        // Vulkan spec: finalLayout must not be VK_IMAGE_LAYOUT_UNDEFINED
        attachmentCreateInfo.nextLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      }
      attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                        ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                        : VK_ATTACHMENT_LOAD_OP_LOAD;
      attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
      vulkan_render_pass_info_add_offscreen_color_attachment(&state->renderPassInfo,
                                                             attachmentCreateInfo);

      // Make sure that finished previous rendering to offscreen texture before writing to color
      // attachment.
      vulkan_render_pass_info_add_execution_barrier(&state->renderPassInfo,
                                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
      vulkan_render_pass_info_add_memory_barrier(&state->renderPassInfo, 0,
                                                 VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }
  }

  if (depthBufferResourceUsed) {
    vulkan_image_render_pass_usage_timeline_debug_print(
        &renderPassElement->depthBufferResource->usageTimeline, 0);

    vulkan_image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_image_render_pass_usage_timeline_get_info(
            &renderPassElement->depthBufferResource->usageTimeline, renderPassElement->idx);

    // Image layout of depth buffer is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL

    vulkan_render_pass_attachment_create_info attachmentCreateInfo = {0};
    attachmentCreateInfo.format = resourceUsageTimelineInfo.currentFormat;
    if (resourceUsageTimelineInfo.previousUsage) {
      // Image layout either undefined or already transitioned at the end of previous render pass.
      attachmentCreateInfo.previousLayout =
          get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    } else {
      attachmentCreateInfo.previousLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    attachmentCreateInfo.currentLayout =
        get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    if (resourceUsageTimelineInfo.nextUsage) {
      attachmentCreateInfo.nextLayout =
          get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.nextUsage);
    } else {
      // Vulkan spec: finalLayout must not be VK_IMAGE_LAYOUT_UNDEFINED
      attachmentCreateInfo.nextLayout =
          get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    }
    attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
    vulkan_render_pass_info_add_depth_attachment(&state->renderPassInfo, attachmentCreateInfo);

    // Make sure that finished previous use of depth buffer before reading and writing from it.
    vulkan_render_pass_info_add_execution_barrier(
        &state->renderPassInfo,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    vulkan_render_pass_info_add_memory_barrier(
        &state->renderPassInfo, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
  }
}

void vulkan_render_graph_compile(vulkan_render_graph *renderGraph) {
  if (renderGraph->_compiledRenderPasses) {
    return;
  }

  vulkan_render_graph_debug_print(renderGraph);
  dl_foreach_elem(vulkan_render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    compile_render_pass(renderPassElement);
  }

  renderGraph->_compiledRenderPasses = true;
}

void vulkan_render_graph_record_commands(vulkan_render_graph *renderGraph,
                                         VkCommandBuffer commandBuffer, size_t swapChainImageIdx) {
  vulkan_render_graph_compile(renderGraph);

  dl_foreach_elem(vulkan_render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    vulkan_render_pass *renderPass = renderPassElement->renderPass;
    vulkan_render_pass_info renderPassInfo = renderPassElement->state.renderPassInfo;

    vulkan_render_pass_record_commands(renderPass, commandBuffer, swapChainImageIdx,
                                       renderPassInfo);
  }
}

void vulkan_render_graph_update(vulkan_render_graph *renderGraph) {
  vulkan_render_graph_compile(renderGraph);
  vulkan_render_pass_state_update(renderGraph->renderPassState);
}

void vulkan_render_graph_send_to_device(vulkan_render_graph *renderGraph) {
  vulkan_render_pass_state_send_to_device(renderGraph->renderPassState);
}

void vulkan_render_graph_debug_print(vulkan_render_graph *renderGraph) {
  log_debug("render graph:\n");
  vulkan_render_pass_state_debug_print(renderGraph->renderPassState, 2);
  log_raw(stdout, "digraph render_graph {");
  dl_foreach_elem(vulkan_render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    if (renderPassElement->next) {
      vulkan_render_graph_render_pass_element_debug_print(renderPassElement);
      log_raw(stdout, " -> ");
      vulkan_render_graph_render_pass_element_debug_print(renderPassElement->next);
      log_raw(stdout, "; ");
    }
    utarray_foreach_elem_deref (vulkan_render_graph_resource *, resource,
                                renderPassElement->offscreenTextures) {
      if (resource->usageTimeline.usages[renderPassElement->idx] &
          vulkan_image_render_pass_usage_read) {
        vulkan_render_graph_resource_debug_print(resource);
        log_raw(stdout, " -> ");
        vulkan_render_graph_render_pass_element_debug_print(renderPassElement);
        log_raw(stdout, "; ");
      }
      if (resource->usageTimeline.usages[renderPassElement->idx] &
          vulkan_image_render_pass_usage_write) {
        vulkan_render_graph_render_pass_element_debug_print(renderPassElement);
        log_raw(stdout, " -> ");
        vulkan_render_graph_resource_debug_print(resource);
        log_raw(stdout, "; ");
      }
    }
  }
  log_raw(stdout, "}\n");
}

vulkan_render_graph_resource *
vulkan_render_graph_add_image_resource(vulkan_render_graph *renderGraph, const char *name,
                                       vulkan_image_type imageType) {
  assert(!renderGraph->_compiledResources);

  dl_foreach_elem(vulkan_render_graph_resource *, existingResourceElement, renderGraph->resources) {
    if (strcmp(existingResourceElement->name, name) == 0) {
      return existingResourceElement;
    }
  }

  assert(imageType != vulkan_image_type_undefined);
  vulkan_render_graph_resource *resourceElement = core_alloc(sizeof(vulkan_render_graph_resource));
  vulkan_render_graph_resource_init(resourceElement, name, imageType);
  DL_APPEND(renderGraph->resources, resourceElement);
  return resourceElement;
}

vulkan_render_graph_resource *
vulkan_render_graph_get_image_resource(vulkan_render_graph *renderGraph, const char *name) {
  dl_foreach_elem(vulkan_render_graph_resource *, existingResourceElement, renderGraph->resources) {
    if (strcmp(existingResourceElement->name, name) == 0) {
      return existingResourceElement;
    }
  }
  UNREACHABLE;
}

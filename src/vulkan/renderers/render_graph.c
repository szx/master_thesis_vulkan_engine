#include "render_graph.h"

/* render graph resource element */

void image_render_pass_usage_timeline_add_new_usage(image_render_pass_usage_timeline *usageTimeline,
                                                    size_t renderGraphIdx,
                                                    image_render_pass_usage usage) {
  usageTimeline->usages[renderGraphIdx] |= usage;
}

void image_render_pass_usage_timeline_add_new_format(
    image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx, VkFormat format) {
  usageTimeline->formats[renderGraphIdx] = format;
}

void image_render_pass_usage_timeline_add_new_clear_value(
    image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkClearValue clearValue) {
  usageTimeline->clearValues[renderGraphIdx] = clearValue;
}

const char *usage_to_str(image_render_pass_usage usage) {
  if (usage == image_render_pass_usage_read) {
    return "R";
  }
  if (usage == image_render_pass_usage_write) {
    return "W";
  }
  if (usage == image_render_pass_usage_read_write) {
    return "RW";
  }
  return "_";
}

void image_render_pass_usage_timeline_debug_print(image_render_pass_usage_timeline *usageTimeline,
                                                  int indent) {
  log_debug(INDENT_FORMAT_STRING "usage timeline:", INDENT_FORMAT_ARGS(0));
  for (size_t i = 0; i < MAX_RENDER_PASS_COUNT; i++) {
    image_render_pass_usage_timeline_info info =
        image_render_pass_usage_timeline_get_info(usageTimeline, i);
    log_debug(INDENT_FORMAT_STRING "%zu:\t%s\t%s\t%s ", INDENT_FORMAT_ARGS(2), i,
              usage_to_str(info.previousUsage), usage_to_str(info.currentUsage),
              usage_to_str(info.nextUsage));
  }
}
image_render_pass_usage_timeline_info
image_render_pass_usage_timeline_get_info(image_render_pass_usage_timeline *usageTimeline,
                                          size_t renderGraphIdx) {
  image_render_pass_usage_timeline_info info = {0};

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

void render_graph_resource_init(render_graph_resource *element, const char *name,
                                image_type imageType) {
  element->name = core_strdup(name);
  element->imageType = imageType;
  element->offscreenTextureIdx = MAX_OFFSCREEN_TEXTURE_COUNT;
  element->usageTimeline = (struct image_render_pass_usage_timeline){0};
  element->prev = NULL;
  element->next = NULL;
}

void render_graph_resource_deinit(render_graph_resource *element) {
  core_free((char *)element->name);
}

void render_graph_resource_debug_print(render_graph_resource *element) {
  log_raw(stdout, "\"render graph resource\\n%p\\n", element);
  log_raw(stdout, "state: \\n");
  log_raw(stdout, "\tname: %s\\n", element->name);
  log_raw(stdout, "\timageType: %s\\n", image_type_debug_str(element->imageType));
  log_raw(stdout, "\" ");
}

/* render graph render pass element */

render_graph_render_pass_element *
render_graph_render_pass_element_create(size_t renderPassIdx, render_pass *renderPass,
                                        render_graph *renderGraph) {
  render_graph_render_pass_element *element = core_alloc(sizeof(render_graph_render_pass_element));

  element->idx = renderPassIdx;
  element->renderPass = renderPass;
  utarray_alloc(element->offscreenTextures, sizeof(render_graph_resource *));
  element->swapChainImageResource = NULL;
  element->depthBufferResource = NULL;

  /* add resources from render pass info */
  for (size_t i = 0; i < renderPass->desc.offscreenFragmentShaderInputCount; i++) {
    offscreen_fragment_shader_input_info *offscreenFragmentShaderInputInfo =
        &renderPass->desc.offscreenFragmentShaderInputs[i];

    render_graph_resource *offscreenTextureResource =
        render_graph_get_image_resource(renderGraph, offscreenFragmentShaderInputInfo->name);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    image_render_pass_usage_timeline_add_new_usage(&offscreenTextureResource->usageTimeline,
                                                   element->idx, image_render_pass_usage_read);
    image_render_pass_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType, 0));
  }

  for (size_t i = 0; i < renderPass->desc.offscreenColorAttachmentCount; i++) {
    offscreen_framebuffer_color_attachment_info *offscreenColorAttachmentInfo =
        &renderPass->desc.offscreenColorAttachments[i];

    render_graph_resource *offscreenTextureResource =
        render_graph_get_image_resource(renderGraph, offscreenColorAttachmentInfo->name);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    image_render_pass_usage_timeline_add_new_usage(&offscreenTextureResource->usageTimeline,
                                                   element->idx, image_render_pass_usage_write);
    image_render_pass_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType, 0));
    image_render_pass_usage_timeline_add_new_clear_value(
        &offscreenTextureResource->usageTimeline, element->idx,
        (VkClearValue){.color = offscreenColorAttachmentInfo->clearValue});
  }

  if (renderPass->desc.useOnscreenColorAttachment) {
    render_graph_resource *swapChainImageResource =
        render_graph_get_image_resource(renderGraph, "swap chain");
    element->swapChainImageResource = swapChainImageResource;

    image_render_pass_usage_timeline_add_new_usage(&swapChainImageResource->usageTimeline,
                                                   element->idx, image_render_pass_usage_write);
    image_render_pass_usage_timeline_add_new_format(
        &swapChainImageResource->usageTimeline, element->idx,
        find_image_format(renderGraph->renderState->vkd, swapChainImageResource->imageType, 0));
    image_render_pass_usage_timeline_add_new_clear_value(
        &swapChainImageResource->usageTimeline, element->idx,
        (VkClearValue){.color = renderPass->desc.onscreenClearValue});
  }

  if (renderPass->desc.offscreenDepthAttachment.name != NULL) {
    render_graph_resource *depthBufferResource = render_graph_get_image_resource(
        renderGraph, renderPass->desc.offscreenDepthAttachment.name);
    element->depthBufferResource = depthBufferResource;

    if (renderPass->desc.offscreenDepthAttachment.depthWriteEnable) {
      image_render_pass_usage_timeline_add_new_usage(&depthBufferResource->usageTimeline,
                                                     element->idx, image_render_pass_usage_write);
    }
    if (renderPass->desc.offscreenDepthAttachment.depthTestEnable) {
      image_render_pass_usage_timeline_add_new_usage(&depthBufferResource->usageTimeline,
                                                     element->idx, image_render_pass_usage_read);
    }
    image_render_pass_usage_timeline_add_new_format(
        &depthBufferResource->usageTimeline, element->idx,
        find_image_format(renderGraph->renderState->vkd, depthBufferResource->imageType, 0));
    image_render_pass_usage_timeline_add_new_clear_value(
        &depthBufferResource->usageTimeline, element->idx,
        (VkClearValue){.depthStencil = renderPass->desc.offscreenDepthAttachment.clearValue});
  }

  element->state = (struct render_graph_render_pass_element_state){0};

  element->prev = NULL;
  element->next = NULL;

  return element;
}

void render_graph_render_pass_element_destroy(render_graph_render_pass_element *element) {
  render_pass_destroy(element->renderPass);
  utarray_free(element->offscreenTextures);
  rendering_info_deinit(&element->state.renderPassInfo);
  core_free(element);
}

void render_graph_render_pass_element_debug_print(render_graph_render_pass_element *element) {
  log_raw(stdout, "\"render graph render pass element\\n%p\\n", element);
  log_raw(stdout, "idx: %zu\\n", element->idx);
  log_raw(stdout, "\" ");
}

/* render graph */

render_graph *render_graph_create(render_state *renderState) {

  render_graph *renderGraph = core_alloc(sizeof(render_graph));

  renderGraph->renderState = renderState;

  renderGraph->renderPassState = render_pass_state_create(renderGraph->renderState);

  renderGraph->renderPassElements = NULL;

  renderGraph->resources = NULL;

  renderGraph->_renderPassIdx = 0;
  renderGraph->_compiledResources = false;
  renderGraph->_compiledRenderPasses = false;

  render_graph_add_image_resource(renderGraph, "swap chain", image_type_swap_chain);

  return renderGraph;
}

void render_graph_destroy(render_graph *renderGraph) {

  dl_foreach_elem(render_graph_resource *, element, renderGraph->resources) {
    render_graph_resource_deinit(element);
    core_free(element);
  }

  dl_foreach_elem(render_graph_render_pass_element *, element, renderGraph->renderPassElements) {
    render_graph_render_pass_element_destroy(element);
  }

  render_pass_state_destroy(renderGraph->renderPassState);

  core_free(renderGraph);
}

void render_graph_start_swap_chain_recreation(render_graph *renderGraph) {

  dl_foreach_elem(render_graph_render_pass_element *, element, renderGraph->renderPassElements) {
    render_pass_deinit(element->renderPass);
  }

  // render_pass_state_deinit(renderer->renderPassState);
}

void render_graph_finish_swap_chain_recreation(render_graph *renderGraph) {

  // render_pass_state_init(renderer->renderPassState, renderer->renderState);
  render_pass_state_reinit_with_new_swap_chain(renderGraph->renderPassState);

  dl_foreach_elem(render_graph_render_pass_element *, element, renderGraph->renderPassElements) {
    render_pass_init(element->renderPass, element->renderPass->desc,
                     element->renderPass->renderState, element->renderPass->renderPassState);
  }
}

/// Add offscreen images to render pass state.
void compile_offscreen_texture_resources(render_graph *renderGraph) {
  dl_foreach_elem(render_graph_resource *, resource, renderGraph->resources) {
    if (resource->imageType >= image_type_offscreen_first &&
        resource->imageType <= image_type_offscreen_count) {
      resource->offscreenTextureIdx = render_pass_state_add_offscreen_texture(
          renderGraph->renderPassState, resource->name, resource->imageType);
    }
  }
}

/// Add offscreen images to render pass state.
void render_pass_desc_add_offscreen_texture_idxes(render_graph *renderGraph,
                                                  render_pass_desc *desc) {
  render_pass_desc_calculate_additional_info(desc);

  dl_foreach_elem(render_graph_resource *, resource, renderGraph->resources) {
    if (resource->imageType >= image_type_offscreen_first &&
        resource->imageType <= image_type_offscreen_count) {
      for (size_t i = 0; i < desc->offscreenFragmentShaderInputCount; i++) {
        assert(desc->offscreenFragmentShaderInputs[i].name);
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

void render_graph_add_render_pass(render_graph *renderGraph, render_pass_desc desc) {
  assert(!renderGraph->_compiledRenderPasses);
  if (!renderGraph->_compiledResources) {
    compile_offscreen_texture_resources(renderGraph);
    renderGraph->_compiledResources = true;
  }
  assert(renderGraph->_renderPassIdx < MAX_RENDER_PASS_COUNT);

  render_pass_desc_add_offscreen_texture_idxes(renderGraph, &desc);

  render_pass *renderPass =
      render_pass_create(desc, renderGraph->renderState, renderGraph->renderPassState);

  render_graph_render_pass_element *renderPassElement = render_graph_render_pass_element_create(
      renderGraph->_renderPassIdx++, renderPass, renderGraph);

  DL_APPEND(renderGraph->renderPassElements, renderPassElement);

  renderGraph->_compiledRenderPasses = false;
}

VkImageLayout get_image_layout_for_swap_chain_image(image_render_pass_usage usage) {
  bool isWriteUsage = (usage & image_render_pass_usage_write) != 0;
  assert((usage & image_render_pass_usage_read) == 0);
  if (isWriteUsage) {
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_depth_buffer(image_render_pass_usage usage) {
  bool isReadUsage = (usage & image_render_pass_usage_read) != 0;
  bool isWriteUsage = (usage & image_render_pass_usage_write) != 0;
  bool isReadWriteUsage = isReadUsage && isWriteUsage;
  if (isReadWriteUsage || isWriteUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
  if (isReadUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_offscreen_texture(image_render_pass_usage usage,
                                                     VkImageAspectFlags aspectFlags) {
  bool isReadUsage = (usage & image_render_pass_usage_read) != 0;
  bool isWriteUsage = (usage & image_render_pass_usage_write) != 0;
  bool isReadWriteUsage = isReadUsage && isWriteUsage;
  if (isReadWriteUsage) {
    // NOTE: Currently VK_IMAGE_LAYOUT_GENERAL is unused.
    assert(0);
    return VK_IMAGE_LAYOUT_GENERAL;
  }
  if (isReadUsage) {
    if (aspectFlags == VK_IMAGE_ASPECT_COLOR_BIT) {
      return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    if (aspectFlags == VK_IMAGE_ASPECT_DEPTH_BIT) {
      return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    }
    UNREACHABLE;
  }
  if (isWriteUsage) {
    if (aspectFlags == VK_IMAGE_ASPECT_COLOR_BIT) {
      return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (aspectFlags == VK_IMAGE_ASPECT_DEPTH_BIT) {
      return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    UNREACHABLE;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

/// Analyze render graph and fill in render pass create info.
void compile_render_pass(render_graph_render_pass_element *renderPassElement) {
  bool swapChainImageResourceUsed = renderPassElement->swapChainImageResource != NULL;
  bool depthBufferResourceUsed = renderPassElement->depthBufferResource != NULL;

  struct render_graph_render_pass_element_state *state = &renderPassElement->state;
  rendering_info_init(&state->renderPassInfo);

  if (swapChainImageResourceUsed) {
    image_render_pass_usage_timeline_debug_print(
        &renderPassElement->swapChainImageResource->usageTimeline, 0);

    image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        image_render_pass_usage_timeline_get_info(
            &renderPassElement->swapChainImageResource->usageTimeline, renderPassElement->idx);
    assert(resourceUsageTimelineInfo.currentUsage == image_render_pass_usage_write);
    assert(resourceUsageTimelineInfo.currentFormat);

    // Image layout of swap chain image is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    rendering_attachment_info attachmentCreateInfo = {0};
    attachmentCreateInfo.name = renderPassElement->swapChainImageResource->name;
    attachmentCreateInfo.currentFormat = resourceUsageTimelineInfo.currentFormat;
    attachmentCreateInfo.currentLayout =
        get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage);
    attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
    rendering_info_add_onscreen_color_attachment(&state->renderPassInfo, attachmentCreateInfo);

    rendering_info_pre_image_layout_transition(
        &state->renderPassInfo,
        (rendering_image_layout_transition_info){
            .name = renderPassElement->swapChainImageResource->name,
            .oldLayout =
                get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.previousUsage),
            .newLayout =
                get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage),
        });
    if (!resourceUsageTimelineInfo.nextUsage) {
      // Vulkan spec: A swapchain’s image must be transitioned to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      // layout before calling vkQueuePresentKHR.
      rendering_info_post_image_layout_transition(
          &state->renderPassInfo, (rendering_image_layout_transition_info){
                                      .name = renderPassElement->swapChainImageResource->name,
                                      .oldLayout = get_image_layout_for_swap_chain_image(
                                          resourceUsageTimelineInfo.currentUsage),
                                      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR});
    }
  }

  utarray_foreach_elem_deref (render_graph_resource *, offscreenTextureResource,
                              renderPassElement->offscreenTextures) {
    image_render_pass_usage_timeline_debug_print(&offscreenTextureResource->usageTimeline, 0);

    image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        image_render_pass_usage_timeline_get_info(&offscreenTextureResource->usageTimeline,
                                                  renderPassElement->idx);

    bool isReadUsage = (resourceUsageTimelineInfo.currentUsage & image_render_pass_usage_read) != 0;
    bool isWriteUsage =
        (resourceUsageTimelineInfo.currentUsage & image_render_pass_usage_write) != 0;

    // NOTE: Forbid reading and writing from the same image in one render pass (despite all cool
    //       stuff that can be done using textureBarrier()).
    assert(!(isReadUsage && isWriteUsage));

    VkImageAspectFlags aspectFlags = find_image_aspects(offscreenTextureResource->imageType);

    if (isReadUsage) {
      // Make sure that finished rendering to offscreen textures before sampling from them in
      // fragment shader.
      rendering_info_pre_image_layout_transition(
          &state->renderPassInfo, (rendering_image_layout_transition_info){
                                      .name = offscreenTextureResource->name,
                                      .oldLayout = get_image_layout_for_offscreen_texture(
                                          resourceUsageTimelineInfo.previousUsage, aspectFlags),
                                      .newLayout = get_image_layout_for_offscreen_texture(
                                          resourceUsageTimelineInfo.currentUsage, aspectFlags),
                                  });
    }
    if (isWriteUsage) {
      // Image layout of offscreen texture is one of following:
      //  - VK_IMAGE_LAYOUT_UNDEFINED
      //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (if used as color attachment)
      //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL (if used as depth attachment )
      //  - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (if color read from fragment shader)
      //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL (if depth read from fragment shader)

      rendering_attachment_info attachmentCreateInfo = {0};
      attachmentCreateInfo.name = offscreenTextureResource->name;
      attachmentCreateInfo.currentFormat = resourceUsageTimelineInfo.currentFormat;
      attachmentCreateInfo.currentLayout = get_image_layout_for_offscreen_texture(
          resourceUsageTimelineInfo.currentUsage, aspectFlags);
      attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                        ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                        : VK_ATTACHMENT_LOAD_OP_LOAD;
      attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
      rendering_info_add_offscreen_color_attachment(&state->renderPassInfo, attachmentCreateInfo);

      rendering_info_pre_image_layout_transition(
          &state->renderPassInfo, (rendering_image_layout_transition_info){
                                      .name = offscreenTextureResource->name,
                                      .oldLayout = get_image_layout_for_offscreen_texture(
                                          resourceUsageTimelineInfo.previousUsage, aspectFlags),
                                      .newLayout = get_image_layout_for_offscreen_texture(
                                          resourceUsageTimelineInfo.currentUsage, aspectFlags),
                                  });
    }
  }

  if (depthBufferResourceUsed) {
    image_render_pass_usage_timeline_debug_print(
        &renderPassElement->depthBufferResource->usageTimeline, 0);

    image_render_pass_usage_timeline_info resourceUsageTimelineInfo =
        image_render_pass_usage_timeline_get_info(
            &renderPassElement->depthBufferResource->usageTimeline, renderPassElement->idx);

    // Image layout of depth attachment is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL

    rendering_attachment_info attachmentCreateInfo = {0};
    attachmentCreateInfo.name = renderPassElement->depthBufferResource->name;
    attachmentCreateInfo.currentFormat = resourceUsageTimelineInfo.currentFormat;
    attachmentCreateInfo.currentLayout =
        get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    attachmentCreateInfo.loadOp = !resourceUsageTimelineInfo.previousUsage
                                      ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                      : VK_ATTACHMENT_LOAD_OP_LOAD;
    attachmentCreateInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentCreateInfo.clearValue = resourceUsageTimelineInfo.clearValue;
    rendering_info_add_depth_attachment(&state->renderPassInfo, attachmentCreateInfo);

    rendering_info_pre_image_layout_transition(
        &state->renderPassInfo,
        (rendering_image_layout_transition_info){
            .name = renderPassElement->depthBufferResource->name,
            .oldLayout = get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.previousUsage),
            .newLayout = get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage),
        });
  }
}

void render_graph_compile(render_graph *renderGraph) {
  if (renderGraph->_compiledRenderPasses) {
    return;
  }

  render_graph_debug_print(renderGraph);
  dl_foreach_elem(render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    compile_render_pass(renderPassElement);
  }

  renderGraph->_compiledRenderPasses = true;
}

void render_graph_record_commands(render_graph *renderGraph, VkCommandBuffer commandBuffer,
                                  size_t swapChainImageIdx) {
  render_graph_compile(renderGraph);

  dl_foreach_elem(render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    render_pass *renderPass = renderPassElement->renderPass;
    rendering_info renderPassInfo = renderPassElement->state.renderPassInfo;

    render_pass_record_commands(renderPass, commandBuffer, swapChainImageIdx, renderPassInfo);
  }
}

void render_graph_update(render_graph *renderGraph) {
  render_graph_compile(renderGraph);
  render_pass_state_update(renderGraph->renderPassState);
}

void render_graph_send_to_device(render_graph *renderGraph) {
  render_pass_state_send_to_device(renderGraph->renderPassState);
}

void render_graph_debug_print(render_graph *renderGraph) {
  log_debug("render graph:\n");
  render_pass_state_debug_print(renderGraph->renderPassState, 2);
  log_raw(stdout, "digraph render_graph {");
  dl_foreach_elem(render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    if (renderPassElement->next) {
      render_graph_render_pass_element_debug_print(renderPassElement);
      log_raw(stdout, " -> ");
      render_graph_render_pass_element_debug_print(renderPassElement->next);
      log_raw(stdout, "; ");
    }
    utarray_foreach_elem_deref (render_graph_resource *, resource,
                                renderPassElement->offscreenTextures) {
      if (resource->usageTimeline.usages[renderPassElement->idx] & image_render_pass_usage_read) {
        render_graph_resource_debug_print(resource);
        log_raw(stdout, " -> ");
        render_graph_render_pass_element_debug_print(renderPassElement);
        log_raw(stdout, "; ");
      }
      if (resource->usageTimeline.usages[renderPassElement->idx] & image_render_pass_usage_write) {
        render_graph_render_pass_element_debug_print(renderPassElement);
        log_raw(stdout, " -> ");
        render_graph_resource_debug_print(resource);
        log_raw(stdout, "; ");
      }
    }
  }
  log_raw(stdout, "}\n");
}

render_graph_resource *render_graph_add_image_resource(render_graph *renderGraph, const char *name,
                                                       image_type imageType) {
  assert(!renderGraph->_compiledResources);

  dl_foreach_elem(render_graph_resource *, existingResourceElement, renderGraph->resources) {
    if (strcmp(existingResourceElement->name, name) == 0) {
      return existingResourceElement;
    }
  }

  assert(imageType != image_type_undefined);
  render_graph_resource *resourceElement = core_alloc(sizeof(render_graph_resource));
  render_graph_resource_init(resourceElement, name, imageType);
  DL_APPEND(renderGraph->resources, resourceElement);
  return resourceElement;
}

render_graph_resource *render_graph_get_image_resource(render_graph *renderGraph,
                                                       const char *name) {
  dl_foreach_elem(render_graph_resource *, existingResourceElement, renderGraph->resources) {
    if (strcmp(existingResourceElement->name, name) == 0) {
      return existingResourceElement;
    }
  }
  UNREACHABLE;
}

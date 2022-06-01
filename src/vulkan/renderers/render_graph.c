#include "render_graph.h"

/* render graph resource element */

void vulkan_render_graph_resource_usage_timeline_add_new_usage(
    vulkan_render_graph_resource_usage_timeline *usageTimeline, size_t renderGraphIdx,
    vulkan_render_graph_resource_usage usage) {
  usageTimeline->usages[renderGraphIdx] |= usage;
}

void vulkan_render_graph_resource_usage_timeline_add_new_format(
    vulkan_render_graph_resource_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkFormat format) {
  usageTimeline->formats[renderGraphIdx] = format;
}

const char *usage_to_str(vulkan_render_graph_resource_usage usage) {
  if (usage == vulkan_render_graph_resource_usage_read) {
    return "R";
  }
  if (usage == vulkan_render_graph_resource_usage_write) {
    return "W";
  }
  if (usage == vulkan_render_graph_resource_usage_read_write) {
    return "RW";
  }
  return "_";
}

void vulkan_render_graph_resource_usage_timeline_debug_print(
    vulkan_render_graph_resource_usage_timeline *usageTimeline, int indent) {
  log_debug(INDENT_FORMAT_STRING "usage timeline:", INDENT_FORMAT_ARGS(0));
  for (size_t i = 0; i < MAX_RENDER_PASS_COUNT; i++) {
    vulkan_render_graph_resource_usage_timeline_info info =
        vulkan_render_graph_resource_usage_timeline_get_info(usageTimeline, i);
    log_debug(INDENT_FORMAT_STRING "%zu:\t%s\t%s\t%s ", INDENT_FORMAT_ARGS(2), i,
              usage_to_str(info.previousUsage), usage_to_str(info.currentUsage),
              usage_to_str(info.nextUsage));
  }
}
vulkan_render_graph_resource_usage_timeline_info
vulkan_render_graph_resource_usage_timeline_get_info(
    vulkan_render_graph_resource_usage_timeline *usageTimeline, size_t renderGraphIdx) {
  vulkan_render_graph_resource_usage_timeline_info info = {0};

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

void vulkan_render_graph_resource_init(vulkan_render_graph_resource *element,
                                       vulkan_render_graph_resource_type type,
                                       vulkan_image_type imageType) {
  element->type = type;
  element->imageType = imageType;
  element->usageTimeline = (struct vulkan_render_graph_resource_usage_timeline){0};
  element->state = (struct vulkan_render_graph_resource_state){0};
  element->prev = NULL;
  element->next = NULL;
}

void vulkan_render_graph_resource_deinit(vulkan_render_graph_resource *element) {
  // No-op.
}

void vulkan_render_graph_resource_clear_compilation_state(vulkan_render_graph_resource *element) {
  element->state = (struct vulkan_render_graph_resource_state){0};
}

void vulkan_render_graph_resource_debug_print(vulkan_render_graph_resource *element) {
  log_raw(stdout, "\"render graph resource\\n%p\\n", element);
  log_raw(stdout, "type: %s\\n", vulkan_render_graph_resource_type_debug_str(element->type));
  log_raw(stdout, "state: \\n");
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
  for (size_t i = 0; i < renderPass->info.offscreenFragmentShaderInputCount; i++) {
    vulkan_offscreen_fragment_shader_input_info *offscreenFragmentShaderInputInfo =
        &renderPass->info.offscreenFragmentShaderInputs[i];

    vulkan_render_graph_resource *offscreenTextureResource =
        vulkan_render_graph_add_image_resource(renderGraph, offscreenFragmentShaderInputInfo->type);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    vulkan_render_graph_resource_usage_timeline_add_new_usage(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_render_graph_resource_usage_read);
    vulkan_render_graph_resource_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType,
                                 0));
  }

  for (size_t i = 0; i < renderPass->info.offscreenColorAttachmentCount; i++) {
    vulkan_offscreen_framebuffer_color_attachment_info *offscreenColorAttachmentInfo =
        &renderPass->info.offscreenColorAttachments[i];

    vulkan_render_graph_resource *offscreenTextureResource =
        vulkan_render_graph_add_image_resource(renderGraph, offscreenColorAttachmentInfo->type);
    utarray_push_back(element->offscreenTextures, &offscreenTextureResource);

    vulkan_render_graph_resource_usage_timeline_add_new_usage(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_render_graph_resource_usage_write);
    vulkan_render_graph_resource_usage_timeline_add_new_format(
        &offscreenTextureResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, offscreenTextureResource->imageType,
                                 0));
  }

  if (renderPass->info.useOnscreenColorAttachment) {
    vulkan_render_graph_resource *swapChainImageResource =
        vulkan_render_graph_add_image_resource(renderGraph, vulkan_image_type_swap_chain);
    element->swapChainImageResource = swapChainImageResource;

    vulkan_render_graph_resource_usage_timeline_add_new_usage(
        &swapChainImageResource->usageTimeline, element->idx,
        vulkan_render_graph_resource_usage_write);
    vulkan_render_graph_resource_usage_timeline_add_new_format(
        &swapChainImageResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, swapChainImageResource->imageType,
                                 0));
  }

  if (renderPass->info.useDepthAttachment) {
    vulkan_render_graph_resource *depthBufferResource =
        vulkan_render_graph_add_image_resource(renderGraph, vulkan_image_type_depth_buffer);
    element->depthBufferResource = depthBufferResource;

    if (renderPass->info.depthAttachmentWriteEnable) {
      vulkan_render_graph_resource_usage_timeline_add_new_usage(
          &depthBufferResource->usageTimeline, element->idx,
          vulkan_render_graph_resource_usage_write);
    }
    if (renderPass->info.depthAttachmentTestEnable) {
      vulkan_render_graph_resource_usage_timeline_add_new_usage(
          &depthBufferResource->usageTimeline, element->idx,
          vulkan_render_graph_resource_usage_read);
    }
    vulkan_render_graph_resource_usage_timeline_add_new_format(
        &depthBufferResource->usageTimeline, element->idx,
        vulkan_find_image_format(renderGraph->renderState->vkd, depthBufferResource->imageType, 0));
  }

  element->state = (struct vulkan_render_graph_render_pass_element_state){0};

  element->prev = NULL;
  element->next = NULL;

  return element;
}

void vulkan_render_graph_render_pass_element_destroy(
    vulkan_render_graph_render_pass_element *element) {

  utarray_free(element->offscreenTextures);
  core_free(element->state.renderPassCreateInfo.onscreenColorAttachmentDescription);
  core_free(element->state.renderPassCreateInfo.onscreenColorAttachmentReference);
  core_free(element->state.renderPassCreateInfo.offscreenColorAttachmentDescriptions);
  core_free(element->state.renderPassCreateInfo.offscreenColorAttachmentReferences);
  core_free(element->state.renderPassCreateInfo.depthAttachmentDescription);
  core_free(element->state.renderPassCreateInfo.depthAttachmentReference);
  core_free(element->state.renderPassCreateInfo.dependencies);
  core_free(element);
}

void vulkan_render_graph_render_pass_element_debug_print(
    vulkan_render_graph_render_pass_element *element) {
  log_raw(stdout, "\"render graph render pass element\\n%p\\n", element);
  log_raw(stdout, "idx: %zu\\n", element->idx);
  log_raw(stdout, "type: %s\\n", vulkan_render_pass_type_debug_str(element->renderPass->type));
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
    vulkan_render_pass_init(element->renderPass, element->renderPass->type,
                            element->renderPass->renderState, element->renderPass->renderPassState);
  }
}

void vulkan_render_graph_add_render_pass(vulkan_render_graph *renderGraph,
                                         vulkan_render_pass_type type) {
  assert(renderGraph->_renderPassIdx < MAX_RENDER_PASS_COUNT);

  vulkan_render_pass *renderPass =
      vulkan_render_pass_create(type, renderGraph->renderState, renderGraph->renderPassState);

  vulkan_render_graph_render_pass_element *renderPassElement =
      vulkan_render_graph_render_pass_element_create(renderGraph->_renderPassIdx++, renderPass,
                                                     renderGraph);

  DL_APPEND(renderGraph->renderPassElements, renderPassElement);
}

void clear_compilation_state(vulkan_render_graph *renderGraph) {
  dl_foreach_elem(vulkan_render_graph_resource *, resource, renderGraph->resources) {
    vulkan_render_graph_resource_clear_compilation_state(resource);
  }
}

VkImageLayout get_image_layout_for_swap_chain_image(vulkan_render_graph_resource_usage usage) {
  bool isWriteUsage = (usage & vulkan_render_graph_resource_usage_write) != 0;
  assert((usage & vulkan_render_graph_resource_usage_read) == 0);
  if (isWriteUsage) {
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_depth_buffer(vulkan_render_graph_resource_usage usage) {
  bool isReadUsage = (usage & vulkan_render_graph_resource_usage_read) != 0;
  bool isWriteUsage = (usage & vulkan_render_graph_resource_usage_write) != 0;
  bool isReadWriteUsage = isReadUsage && isWriteUsage;
  if (isReadWriteUsage || isWriteUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  }
  if (isReadUsage) {
    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  }
  return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageLayout get_image_layout_for_offscreen_texture(vulkan_render_graph_resource_usage usage) {
  bool isReadUsage = (usage & vulkan_render_graph_resource_usage_read) != 0;
  bool isWriteUsage = (usage & vulkan_render_graph_resource_usage_write) != 0;
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

void compile_render_pass(vulkan_render_graph_render_pass_element *renderPassElement) {

  VkAttachmentDescription *onscreenColorAttachmentDescription = NULL;
  VkAttachmentReference *onscreenColorAttachmentReference = NULL;

  size_t offscreenColorAttachmentDescriptionCount = 0;
  VkAttachmentDescription *offscreenColorAttachmentDescriptions = NULL;

  size_t offscreenColorAttachmentReferenceCount = 0;
  VkAttachmentReference *offscreenColorAttachmentReferences = NULL;

  VkAttachmentDescription *depthAttachmentDescription = NULL;
  VkAttachmentReference *depthAttachmentReference = NULL;

  size_t dependencyCount = 1;
  VkSubpassDependency *dependencies = core_alloc(dependencyCount * sizeof(VkSubpassDependency));

  /* Analyze render graph. */

  uint32_t attachmentIdx = 0;
  bool swapChainImageResourceUsed = renderPassElement->swapChainImageResource != NULL;
  bool depthBufferResourceUsed = renderPassElement->depthBufferResource != NULL;
  size_t writtenOffscreenTextureCount = 0; // HIRO CONTINUE refactor out

  VkPipelineStageFlags srcStageMask = 0;
  VkPipelineStageFlags dstStageMask = 0;
  VkAccessFlags srcAccessMask = 0;
  VkAccessFlags dstAccessMask = 0;
  VkDependencyFlags dependencyFlags = 0;
// Execution dependency - stages in dstStageMask (and later) will not start until all stages in
// srcStageMask (and earlier) are complete.
#define ADD_EXECUTION_BARRIER(_srcStageMask, _dstStageMask)                                        \
  do {                                                                                             \
    srcStageMask |= (_srcStageMask);                                                               \
    dstStageMask |= (_dstStageMask);                                                               \
  } while (0)

// Memory dependency - source accesses defined by srcAccessMask are visible and available to
// destination accesses defined by dstAccessMask.
// NOTE: *_READ_BIT in srcAccessMask is unnecessary - source mask determine visibility of write
//       accesses (see https://github.com/KhronosGroup/Vulkan-Docs/issues/131).
#define ADD_MEMORY_BARRIER(_srcAccessMask, _dstAccessMask)                                         \
  do {                                                                                             \
    srcAccessMask |= (_srcAccessMask);                                                             \
    dstAccessMask |= (_dstAccessMask);                                                             \
  } while (0)

  if (swapChainImageResourceUsed) {
    vulkan_render_graph_resource_usage_timeline_debug_print(
        &renderPassElement->swapChainImageResource->usageTimeline, 0);

    vulkan_render_graph_resource_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_render_graph_resource_usage_timeline_get_info(
            &renderPassElement->swapChainImageResource->usageTimeline, renderPassElement->idx);
    assert(resourceUsageTimelineInfo.currentUsage == vulkan_render_graph_resource_usage_write);
    assert(resourceUsageTimelineInfo.currentFormat);

    struct vulkan_render_graph_resource_state *state =
        &renderPassElement->swapChainImageResource->state;

    // Image layout of swap chain image is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

    state->isFramebufferAttachment = true;
    state->format = resourceUsageTimelineInfo.currentFormat;

    if (resourceUsageTimelineInfo.previousUsage) {
      // Image layout either undefined or already transitioned at the end of previous render pass.
      state->initialLayout =
          get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage);
    } else {
      state->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    state->currentLayout =
        get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.currentUsage);
    if (resourceUsageTimelineInfo.nextUsage) {
      state->finalLayout =
          get_image_layout_for_swap_chain_image(resourceUsageTimelineInfo.nextUsage);
    } else {
      // Vulkan spec: A swapchain’s image must be transitioned to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      // layout before calling vkQueuePresentKHR.
      state->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    state->loadOp = !resourceUsageTimelineInfo.previousUsage ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                             : VK_ATTACHMENT_LOAD_OP_LOAD;
    state->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    state->attachmentIdx = attachmentIdx++;

    // Make sure that finished previous rendering to onscreen texture before writing to color
    // attachment.
    ADD_EXECUTION_BARRIER(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    ADD_MEMORY_BARRIER(0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
  }

  utarray_foreach_elem_deref (vulkan_render_graph_resource *, offscreenTextureResource,
                              renderPassElement->offscreenTextures) {
    vulkan_render_graph_resource_usage_timeline_debug_print(
        &offscreenTextureResource->usageTimeline, 0);

    vulkan_render_graph_resource_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_render_graph_resource_usage_timeline_get_info(
            &offscreenTextureResource->usageTimeline, renderPassElement->idx);

    struct vulkan_render_graph_resource_state *state = &offscreenTextureResource->state;

    bool isReadUsage =
        (resourceUsageTimelineInfo.currentUsage & vulkan_render_graph_resource_usage_read) != 0;
    bool isWriteUsage =
        (resourceUsageTimelineInfo.currentUsage & vulkan_render_graph_resource_usage_write) != 0;

    if (isReadUsage) {
      // Make sure that finished rendering to offscreen textures before sampling from them in
      // fragment shader.
      ADD_EXECUTION_BARRIER(VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      ADD_MEMORY_BARRIER(0, VK_ACCESS_SHADER_READ_BIT);
    }

    if (isWriteUsage) {
      writtenOffscreenTextureCount++;

      // Image layout of offscreen texture is one of following:
      //  - VK_IMAGE_LAYOUT_UNDEFINED
      //  - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (if used as framebuffer color attachment)
      //  - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (if read from fragment shader)

      state->isFramebufferAttachment = true;
      state->format = resourceUsageTimelineInfo.currentFormat;

      if (resourceUsageTimelineInfo.previousUsage) {
        // Image layout either undefined or already transitioned at the end of previous render pass.
        state->initialLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      } else {
        state->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      }
      state->currentLayout =
          get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      if (resourceUsageTimelineInfo.nextUsage) {
        state->finalLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.nextUsage);
      } else {
        // Vulkan spec: finalLayout must not be VK_IMAGE_LAYOUT_UNDEFINED
        state->finalLayout =
            get_image_layout_for_offscreen_texture(resourceUsageTimelineInfo.currentUsage);
      }

      state->loadOp = !resourceUsageTimelineInfo.previousUsage ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                               : VK_ATTACHMENT_LOAD_OP_LOAD;
      state->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

      state->attachmentIdx = attachmentIdx++;

      // Make sure that finished previous rendering to offscreen texture before writing to color
      // attachment.
      ADD_EXECUTION_BARRIER(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
      ADD_MEMORY_BARRIER(0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }
  }

  if (depthBufferResourceUsed) {
    vulkan_render_graph_resource_usage_timeline_debug_print(
        &renderPassElement->depthBufferResource->usageTimeline, 0);

    vulkan_render_graph_resource_usage_timeline_info resourceUsageTimelineInfo =
        vulkan_render_graph_resource_usage_timeline_get_info(
            &renderPassElement->depthBufferResource->usageTimeline, renderPassElement->idx);

    struct vulkan_render_graph_resource_state *state =
        &renderPassElement->depthBufferResource->state;

    // Image layout of depth buffer is one of following:
    //  - VK_IMAGE_LAYOUT_UNDEFINED
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    //  - VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL

    state->isFramebufferAttachment = true;
    state->format = resourceUsageTimelineInfo.currentFormat;

    if (resourceUsageTimelineInfo.previousUsage) {
      // Image layout either undefined or already transitioned at the end of previous render pass.
      state->initialLayout =
          get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    } else {
      state->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }
    state->currentLayout =
        get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    if (resourceUsageTimelineInfo.nextUsage) {
      state->finalLayout = get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.nextUsage);
    } else {
      // Vulkan spec: finalLayout must not be VK_IMAGE_LAYOUT_UNDEFINED
      state->finalLayout =
          get_image_layout_for_depth_buffer(resourceUsageTimelineInfo.currentUsage);
    }

    state->loadOp = !resourceUsageTimelineInfo.previousUsage ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                             : VK_ATTACHMENT_LOAD_OP_LOAD;
    state->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    state->attachmentIdx = attachmentIdx++;

    // Make sure that finished previous use of depth buffer before reading and writing from it.
    ADD_EXECUTION_BARRIER(
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    ADD_MEMORY_BARRIER(VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
  }

  /* Fill in render pass create info. */

  if (swapChainImageResourceUsed) {
    struct vulkan_render_graph_resource_state *state =
        &renderPassElement->swapChainImageResource->state;

    // HIRO CONTINUE refactor
    assert(state->isFramebufferAttachment);
    onscreenColorAttachmentDescription = core_alloc(sizeof(VkAttachmentDescription));
    *onscreenColorAttachmentDescription = (VkAttachmentDescription){
        .format = state->format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = state->loadOp,
        .storeOp = state->storeOp,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = state->initialLayout,
        .finalLayout = state->finalLayout,
    };
    onscreenColorAttachmentReference = core_alloc(sizeof(VkAttachmentReference));
    *onscreenColorAttachmentReference = (VkAttachmentReference){
        .attachment = state->attachmentIdx,
        .layout = state->currentLayout,
    };
  }

  if (depthBufferResourceUsed) {
    struct vulkan_render_graph_resource_state *state =
        &renderPassElement->depthBufferResource->state;

    assert(state->isFramebufferAttachment);
    depthAttachmentDescription = core_alloc(sizeof(VkAttachmentDescription));
    *depthAttachmentDescription = (VkAttachmentDescription){
        .format = state->format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = state->loadOp,
        .storeOp = state->storeOp,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = state->initialLayout,
        .finalLayout = state->finalLayout,
    };
    depthAttachmentReference = core_alloc(sizeof(VkAttachmentReference));
    *depthAttachmentReference = (VkAttachmentReference){
        .attachment = state->attachmentIdx,
        .layout = state->currentLayout,
    };
  }

  offscreenColorAttachmentDescriptionCount = writtenOffscreenTextureCount;
  offscreenColorAttachmentDescriptions =
      core_alloc(offscreenColorAttachmentDescriptionCount * sizeof(VkAttachmentDescription));
  offscreenColorAttachmentReferenceCount = writtenOffscreenTextureCount;
  offscreenColorAttachmentReferences =
      core_alloc(offscreenColorAttachmentReferenceCount * sizeof(VkAttachmentReference));
  size_t i = 0;
  utarray_foreach_elem_deref (vulkan_render_graph_resource *, offscreenTextureResource,
                              renderPassElement->offscreenTextures) {
    struct vulkan_render_graph_resource_state *state = &offscreenTextureResource->state;
    if (!offscreenTextureResource->state.isFramebufferAttachment) {
      continue;
    }
    offscreenColorAttachmentDescriptions[i] = (VkAttachmentDescription){
        .format = state->format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = state->loadOp,
        .storeOp = state->storeOp,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = state->initialLayout,
        .finalLayout = state->finalLayout,
    };
    offscreenColorAttachmentReferences[i] = (VkAttachmentReference){
        .attachment = state->attachmentIdx,
        .layout = state->currentLayout,
    };
    i++;
  }

  dependencies[0] = (VkSubpassDependency){
      // Explicit external subpass dependency.
      // Same behaviour as VkImageMemoryBarrier before render pass.
      // See also:
      // https://www.lunarg.com/wp-content/uploads/2021/01/Final_Guide-to-Vulkan-Synchronization-Validation_Jan_21.pdf
      // https://www.khronos.org/blog/understanding-vulkan-synchronization
      // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
      .srcSubpass = VK_SUBPASS_EXTERNAL,  .dstSubpass = 0,
      .srcStageMask = srcStageMask,       .dstStageMask = dstStageMask,
      .srcAccessMask = srcAccessMask,     .dstAccessMask = dstAccessMask,
      .dependencyFlags = dependencyFlags,
  };

  renderPassElement->state.renderPassCreateInfo = (vulkan_render_pass_create_info){
      .onscreenColorAttachmentDescription = onscreenColorAttachmentDescription,
      .onscreenColorAttachmentReference = onscreenColorAttachmentReference,

      .offscreenColorAttachmentDescriptionCount = offscreenColorAttachmentDescriptionCount,
      .offscreenColorAttachmentDescriptions = offscreenColorAttachmentDescriptions,
      .offscreenColorAttachmentReferenceCount = offscreenColorAttachmentReferenceCount,
      .offscreenColorAttachmentReferences = offscreenColorAttachmentReferences,

      .depthAttachmentDescription = depthAttachmentDescription,
      .depthAttachmentReference = depthAttachmentReference,

      .dependencyCount = dependencyCount,
      .dependencies = dependencies,
  };
}

void vulkan_render_graph_compile(vulkan_render_graph *renderGraph) {
  vulkan_render_graph_debug_print(renderGraph);
  dl_foreach_elem(vulkan_render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    clear_compilation_state(renderGraph);
    compile_render_pass(renderPassElement);
  }
}

void vulkan_render_graph_record_commands(vulkan_render_graph *renderGraph,
                                         VkCommandBuffer commandBuffer, size_t swapChainImageIdx) {
  size_t currentFrameInFlight = renderGraph->renderState->sync->currentFrameInFlight;
  vulkan_render_pass_frame_state *frameState =
      vulkan_render_pass_state_get_frame_state(renderGraph->renderPassState, currentFrameInFlight);

  dl_foreach_elem(vulkan_render_graph_render_pass_element *, renderPassElement,
                  renderGraph->renderPassElements) {
    vulkan_render_pass *renderPass = renderPassElement->renderPass;
    vulkan_render_pass_create_info renderPassCreateInfo =
        renderPassElement->state.renderPassCreateInfo;

    vulkan_render_pass_record_commands(renderPass, commandBuffer, swapChainImageIdx,
                                       renderPassCreateInfo);
  }
}

void vulkan_render_graph_update(vulkan_render_graph *renderGraph) {
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
          vulkan_render_graph_resource_usage_read) {
        vulkan_render_graph_resource_debug_print(resource);
        log_raw(stdout, " -> ");
        vulkan_render_graph_render_pass_element_debug_print(renderPassElement);
        log_raw(stdout, "; ");
      }
      if (resource->usageTimeline.usages[renderPassElement->idx] &
          vulkan_render_graph_resource_usage_write) {
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
vulkan_render_graph_add_image_resource(vulkan_render_graph *renderGraph,
                                       vulkan_image_type imageType) {
  vulkan_render_graph_resource_type resourceType;
  if (imageType == vulkan_image_type_swap_chain) {
    resourceType = vulkan_render_graph_resource_type_swap_chain_image;
  } else if (imageType == vulkan_image_type_depth_buffer) {
    resourceType = vulkan_render_graph_resource_type_depth_buffer;
  } else {
    resourceType = vulkan_render_graph_resource_type_offscreen_texture;
  }

  dl_foreach_elem(vulkan_render_graph_resource *, existingResourceElement, renderGraph->resources) {
    if (existingResourceElement->type == resourceType &&
        existingResourceElement->imageType == imageType) {
      return existingResourceElement;
    }
  }

  vulkan_render_graph_resource *resourceElement = core_alloc(sizeof(vulkan_render_graph_resource));
  vulkan_render_graph_resource_init(resourceElement, resourceType, imageType);
  DL_APPEND(renderGraph->resources, resourceElement);
  return resourceElement;
}

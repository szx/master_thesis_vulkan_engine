#include "render_pass_decls.h"
#include "render_pass.h"

uint32_t
vulkan_render_pass_info_get_framebuffer_attachment_count(vulkan_render_pass_info renderPassInfo) {
  uint32_t count =
      (renderPassInfo.useOnscreenColorAttachment ? 1 : 0) +
      vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo) +
      (renderPassInfo.useDepthAttachment ? 1 : 0);
  assert(count > 0);
  return count;
}

uint32_t vulkan_render_pass_info_get_framebuffer_color_attachment_count(
    vulkan_render_pass_info renderPassInfo) {
  uint32_t count =
      (renderPassInfo.useOnscreenColorAttachment ? 1 : 0) +
      vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo);
  assert(count > 0);
  return count;
}

uint32_t vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(
    vulkan_render_pass_info renderPassInfo) {
  uint32_t count = 0;
  for (size_t i = 0; i < renderPassInfo.offscreenTextureCount; i++) {
    if (renderPassInfo.offscreenTextureInfos[i].usage ==
        vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
      count++;
    }
  }
  return count;
}

void vulkan_render_pass_info_get_framebuffer_offscreen_attachment_image_views(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState,
    VkImageView *offscreenImageViews) {
  size_t imageViewIdx = 0;
  for (size_t i = 0; i < renderPassInfo.offscreenTextureCount; i++) {
    vulkan_render_pass_offscreen_texture_info offscreenTextureInfo =
        renderPassInfo.offscreenTextureInfos[i];
    if (offscreenTextureInfo.usage == vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
      offscreenImageViews[imageViewIdx++] =
          vulkan_render_pass_frame_state_get_offscreen_framebuffer_attachment_image(
              frameState, offscreenTextureInfo.type)
              ->imageView;
    }
  }
}

VkImageView vulkan_render_pass_info_get_framebuffer_depth_attachment_image_view(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState) {
  return frameState->depthBufferImage->imageView;
}

void vulkan_render_pass_info_get_framebuffer_offscreen_attachment_formats(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState,
    VkFormat *offscreenFormats) {
  size_t formatIdx = 0;
  for (size_t i = 0; i < renderPassInfo.offscreenTextureCount; i++) {
    vulkan_render_pass_offscreen_texture_info offscreenTextureInfo =
        renderPassInfo.offscreenTextureInfos[i];
    if (offscreenTextureInfo.usage == vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
      offscreenFormats[formatIdx++] =
          vulkan_render_pass_frame_state_get_offscreen_framebuffer_attachment_image(
              frameState, offscreenTextureInfo.type)
              ->format;
    }
  }
}

VkFormat vulkan_render_pass_info_get_framebuffer_depth_attachment_formats(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState) {
  return frameState->depthBufferImage->format;
}

void vulkan_render_pass_info_get_framebuffer_attachment_image_views(
    vulkan_render_pass_info renderPassInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments) {
  size_t idx = 0;
  if (renderPassInfo.useOnscreenColorAttachment) {
    assert(swapChainImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = swapChainImageView;
  }
  for (size_t i = 0;
       i < vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo);
       i++) {
    assert(offscreenImageViews != NULL);
    framebufferAttachments[idx++] = offscreenImageViews[i];
  }
  if (renderPassInfo.useDepthAttachment) {
    assert(depthBufferImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = depthBufferImageView;
  }
}

void vulkan_render_pass_info_get_framebuffer_attachment_clear_values(
    vulkan_render_pass_info renderPassInfo, VkClearValue *clearValues) {
  size_t idx = 0;
  if (renderPassInfo.useOnscreenColorAttachment) {
    clearValues[idx++].color = renderPassInfo.onscreenClearValue;
  }
  for (size_t i = 0;
       i < vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(renderPassInfo);
       i++) {
    clearValues[idx++].color = renderPassInfo.offscreenClearValue;
  }
  if (renderPassInfo.useDepthAttachment) {
    clearValues[idx++].depthStencil = renderPassInfo.depthClearValue;
  }
}

uint32_t vulkan_render_pass_info_get_dependency_count(vulkan_render_pass_info renderPassInfo) {
  return 1;
}

void vulkan_render_pass_info_get_render_pass_create_info(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass *prev, vulkan_render_pass *next,
    VkFormat swapChainImageFormat, VkFormat *offscreenImageFormats, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference, VkSubpassDependency *dependencies) {

  /* analyze renderPass chain */
#define ITERATE_INFO(_start, _body)                                                                \
  {                                                                                                \
    vulkan_render_pass *it = _start;                                                               \
    while (it != NULL) {                                                                           \
      vulkan_render_pass_info info = vulkan_render_pass_get_renderPass_info(it);                   \
      _body it = it->_start;                                                                       \
    }                                                                                              \
  }

  VkSubpassDependency dependency0 = {
      // Explicit external subpass dependency.
      // Same behaviour as VkImageMemoryBarrier before render pass.
      // See also:
      // https://www.lunarg.com/wp-content/uploads/2021/01/Final_Guide-to-Vulkan-Synchronization-Validation_Jan_21.pdf
      // https://www.khronos.org/blog/understanding-vulkan-synchronization
      // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
  };
  VkSubpassDependency dependency1 = {
      .srcSubpass = 0,
      .dstSubpass = VK_SUBPASS_EXTERNAL,
  };

  // Execution dependency - stages in dstStageMask (and later) will not start until all stages in
  // srcStageMask (and earlier) are complete.
#define ADD_EXECUTION_BARRIER(_dependency, _srcStageMask, _dstStageMask)                           \
  do {                                                                                             \
    (_dependency).srcStageMask |= (_srcStageMask);                                                 \
    (_dependency).dstStageMask |= (_dstStageMask);                                                 \
  } while (0)

  // Memory dependency - source accesses defined by srcAccessMask are visible and available to
  // destination accesses defined by dstAccessMask.
  // NOTE: *_READ_BIT in srcAccessMask is unnecessary - source mask determine visibility of write
  //       accesses (see https://github.com/KhronosGroup/Vulkan-Docs/issues/131).
#define ADD_MEMORY_BARRIER(_dependency, _srcAccessMask, _dstAccessMask)                            \
  do {                                                                                             \
    (_dependency).srcAccessMask |= (_srcAccessMask);                                               \
    (_dependency).dstAccessMask |= (_dstAccessMask);                                               \
  } while (0)

  /* prepare attachment info and subpass dependency */
  size_t idx = 0;
  if (renderPassInfo.useOnscreenColorAttachment) {

    size_t prevOnscreenColorAttachmentCount = 0;
    size_t nextOnscreenColorAttachmentCount = 0;
    ITERATE_INFO(
        prev, if (info.useOnscreenColorAttachment) { prevOnscreenColorAttachmentCount++; })
    ITERATE_INFO(
        next, if (info.useOnscreenColorAttachment) { nextOnscreenColorAttachmentCount++; })
    bool isFirstOnscreenPipeline =
        renderPassInfo.useOnscreenColorAttachment && prevOnscreenColorAttachmentCount == 0;
    bool isLastOnscreenPipeline =
        renderPassInfo.useOnscreenColorAttachment && nextOnscreenColorAttachmentCount == 0;

    VkAttachmentLoadOp loadOp;
    if (isFirstOnscreenPipeline) {
      loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    } else {
      loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    // NOTE: Image layout of onscreen attachment is either VK_IMAGE_LAYOUT_UNDEFINED or
    //       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.

    VkImageLayout initialLayout;
    if (isFirstOnscreenPipeline) {
      initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    } else {
      initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkImageLayout finalLayout;
    if (isLastOnscreenPipeline) {
      finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } else {
      finalLayout = layout;
    }

    *onscreenColorAttachmentDescription = (VkAttachmentDescription){
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = loadOp,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = initialLayout,
        .finalLayout = finalLayout,
    };
    *onscreenColorAttachmentReference = (VkAttachmentReference){
        .attachment = idx++,
        .layout = layout,
    };

    // Make sure that finished rendering to onscreen texture before sampling from them in
    // fragment shader.
    ADD_EXECUTION_BARRIER(dependency0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    ADD_MEMORY_BARRIER(dependency0, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
  }

  for (size_t i = 0; i < renderPassInfo.offscreenTextureCount; i++) {
    vulkan_render_pass_offscreen_texture_info offscreenTextureInfo =
        renderPassInfo.offscreenTextureInfos[i];

    if (offscreenTextureInfo.usage == vulkan_offscreen_texture_usage_fragment_shader_read) {
      // Make sure that finished rendering to offscreen textures before sampling from them in
      // fragment shader.
      ADD_EXECUTION_BARRIER(dependency0, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
      ADD_MEMORY_BARRIER(dependency0, 0, VK_ACCESS_SHADER_READ_BIT);
    }

    if (offscreenTextureInfo.usage == vulkan_offscreen_texture_usage_framebuffer_color_attachment) {

      VkFormat offscreenImageFormat = offscreenImageFormats[i];

      size_t prevOffscreenTextureInfoCount = 0;
      size_t nextOffscreenTextureInfoCount = 0;
      vulkan_render_pass_offscreen_texture_info nextOffscreenTextureInfo = {0};
      ITERATE_INFO(
          prev, if (info.offscreenTextureCount > 0) {
            for (size_t typeIdx = 0; typeIdx < info.offscreenTextureCount; typeIdx++) {
              vulkan_render_pass_offscreen_texture_info prevInfo =
                  info.offscreenTextureInfos[typeIdx];
              if (prevInfo.type == offscreenTextureInfo.type) {
                prevOffscreenTextureInfoCount++;
              }
            }
          })
      ITERATE_INFO(
          next, if (info.offscreenTextureCount > 0) {
            for (size_t typeIdx = 0; typeIdx < info.offscreenTextureCount; typeIdx++) {
              vulkan_render_pass_offscreen_texture_info nextInfo =
                  info.offscreenTextureInfos[typeIdx];
              if (nextInfo.type == offscreenTextureInfo.type) {
                nextOffscreenTextureInfoCount++;
                if (nextOffscreenTextureInfoCount == 1) {
                  nextOffscreenTextureInfo = nextInfo;
                }
              }
            }
          })

      bool isFirstOffscreenPipeline = prevOffscreenTextureInfoCount == 0;
      bool isLastOffscreenPipeline = nextOffscreenTextureInfoCount == 0;

      VkAttachmentLoadOp loadOp;
      if (isFirstOffscreenPipeline) {
        loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      } else {
        loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
      }

      // NOTE: Image layout of offscreen texture is either:
      //        - VK_IMAGE_LAYOUT_UNDEFINED
      //        - VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (if used as framebuffer color attachment)
      //        - VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (if read from fragment shader)

      VkImageLayout initialLayout;
      if (isFirstOffscreenPipeline) {
        initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      } else {
        // NOTE: Current initial layout is equal to current layout, because appropriate image layout
        //       transition occurred in previous render pass.
        if (offscreenTextureInfo.usage ==
            vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
          initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (offscreenTextureInfo.usage ==
                   vulkan_offscreen_texture_usage_fragment_shader_read) {
          initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else {
          UNREACHABLE;
        }
      }

      VkImageLayout layout;
      if (offscreenTextureInfo.usage ==
          vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
        layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      } else if (offscreenTextureInfo.usage ==
                 vulkan_offscreen_texture_usage_fragment_shader_read) {
        layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      } else {
        UNREACHABLE;
      }

      VkImageLayout finalLayout;
      if (isLastOffscreenPipeline) {
        finalLayout = layout;
      } else {
        assert(nextOffscreenTextureInfoCount > 0);
        if (nextOffscreenTextureInfo.usage ==
            vulkan_offscreen_texture_usage_framebuffer_color_attachment) {
          finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } else if (nextOffscreenTextureInfo.usage ==
                   vulkan_offscreen_texture_usage_fragment_shader_read) {
          finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else {
          UNREACHABLE;
        }
      }

      VkAttachmentDescription description = (VkAttachmentDescription){
          .format = offscreenImageFormat,
          .samples = VK_SAMPLE_COUNT_1_BIT,
          .loadOp = loadOp,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
          .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
          .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
          .initialLayout = initialLayout,
          .finalLayout = finalLayout,
      };
      offscreenColorAttachmentDescriptions[i] = description;
      offscreenColorAttachmentReferences[i] = (VkAttachmentReference){
          .attachment = idx++,
          .layout = layout,
      };

      ADD_EXECUTION_BARRIER(dependency0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
      ADD_MEMORY_BARRIER(dependency0, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    }
  }

  if (renderPassInfo.useDepthAttachment) {
    bool prevUseDepthAttachment = false;
    bool nextUseDepthAttachment = false;
    ITERATE_INFO(
        prev, if (info.useDepthAttachment) {
          prevUseDepthAttachment = true;
          break;
        })
    ITERATE_INFO(
        next, if (info.useDepthAttachment) {
          nextUseDepthAttachment = true;
          break;
        })
    bool isFirstDepthAttachment = !prevUseDepthAttachment;
    bool isLastDepthAttachment = !nextUseDepthAttachment;

    VkImageLayout initialLayout;
    if (isFirstDepthAttachment) {
      initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    } else {
      initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    *depthAttachmentDescription = (VkAttachmentDescription){
        .format = depthBufferImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = renderPassInfo.depthAttachmentLoadOp,
        .storeOp = renderPassInfo.depthAttachmentStoreOp,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = initialLayout,
        .finalLayout = finalLayout,
    };
    *depthAttachmentReference = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    // Make sure that finished previous use of depth buffer before reading and writing from it.
    ADD_EXECUTION_BARRIER(
        dependency0,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    ADD_MEMORY_BARRIER(dependency0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                       VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
  }

  /*
  // Full renderPass barrier (use for debugging).
  ADD_EXECUTION_BARRIER(dependency0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);
  ADD_MEMORY_BARRIER(dependency0, VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
                     VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT);
  */

  dependencies[0] = dependency0;

#undef ITERATE_INFO
}

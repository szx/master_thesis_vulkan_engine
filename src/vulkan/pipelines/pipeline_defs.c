#include "pipeline_defs.h"
#include "pipeline.h"

uint32_t vulkan_pipeline_info_get_framebuffer_attachment_count(vulkan_pipeline_info pipelineInfo) {
  uint32_t count = (pipelineInfo.useOnscreenColorAttachment ? 1 : 0) +
                   pipelineInfo.offscreenColorAttachmentCount +
                   (pipelineInfo.useDepthAttachment ? 1 : 0);
  assert(count > 0);
  return count;
}

void vulkan_pipeline_info_get_framebuffer_attachment_image_views(
    vulkan_pipeline_info pipelineInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments) {
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {
    assert(swapChainImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = swapChainImageView;
  }
  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {
    framebufferAttachments[idx++] = offscreenImageViews[i];
  }
  if (pipelineInfo.useDepthAttachment) {
    assert(depthBufferImageView != VK_NULL_HANDLE);
    framebufferAttachments[idx++] = depthBufferImageView;
  }
}

void vulkan_pipeline_info_get_framebuffer_attachment_clear_values(vulkan_pipeline_info pipelineInfo,
                                                                  VkClearValue *clearValues) {
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {
    clearValues[idx++].color = pipelineInfo.onscreenClearValue;
  }
  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {
    clearValues[idx++].color = pipelineInfo.offscreenClearValue;
  }
  if (pipelineInfo.useDepthAttachment) {
    clearValues[idx++].depthStencil = pipelineInfo.depthClearValue;
  }
}

void vulkan_pipeline_info_get_render_pass_create_info(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline *prev, vulkan_pipeline *next,
    VkFormat swapChainImageFormat, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference) {

  /* analyze pipeline chain */
#define ITERATE_INFO(_start, _body)                                                                \
  {                                                                                                \
    vulkan_pipeline *it = _start;                                                                  \
    while (it != NULL) {                                                                           \
      vulkan_pipeline_info info = vulkan_pipeline_get_pipeline_info(it);                           \
      _body it = it->_start;                                                                       \
    }                                                                                              \
  }

  /* prepare attachment info */
  size_t idx = 0;
  if (pipelineInfo.useOnscreenColorAttachment) {

    size_t prevOnscreenColorAttachmentCount = 0;
    size_t nextOnscreenColorAttachmentCount = 0;
    ITERATE_INFO(
        prev, if (info.useOnscreenColorAttachment) { prevOnscreenColorAttachmentCount++; })
    ITERATE_INFO(
        next, if (info.useOnscreenColorAttachment) { nextOnscreenColorAttachmentCount++; })
    bool isFirstOnscreenPipeline =
        pipelineInfo.useOnscreenColorAttachment && prevOnscreenColorAttachmentCount == 0;
    bool isLastOnscreenPipeline =
        pipelineInfo.useOnscreenColorAttachment && nextOnscreenColorAttachmentCount == 0;

    VkAttachmentLoadOp loadOp;
    if (isFirstOnscreenPipeline) {
      loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    } else {
      loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    VkImageLayout initialLayout;
    if (isFirstOnscreenPipeline) {
      initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    } else {
      initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkImageLayout finalLayout;
    if (isLastOnscreenPipeline) {
      finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } else {
      finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
  }

  for (size_t i = 0; i < pipelineInfo.offscreenColorAttachmentCount; i++) {

    size_t prevOffscreenColorAttachmentCount = 0;
    size_t nextOffscreenColorAttachmentCount = 0;
    ITERATE_INFO(
        prev, if (info.offscreenColorAttachmentCount > 0) {
          for (size_t typeIdx = 0; typeIdx < info.offscreenColorAttachmentCount; typeIdx++) {
            if (info.offscreenColorAttachmentTypes[typeIdx] ==
                pipelineInfo.offscreenColorAttachmentTypes[i]) {
              prevOffscreenColorAttachmentCount++;
            }
          }
        })
    ITERATE_INFO(
        next, if (info.offscreenColorAttachmentCount > 0) {
          for (size_t typeIdx = 0; typeIdx < info.offscreenColorAttachmentCount; typeIdx++) {
            if (info.offscreenColorAttachmentTypes[typeIdx] ==
                pipelineInfo.offscreenColorAttachmentTypes[i]) {
              nextOffscreenColorAttachmentCount++;
            }
          }
        })

    bool isFirstOffscreenPipeline =
        pipelineInfo.useOnscreenColorAttachment && prevOffscreenColorAttachmentCount == 0;

    VkAttachmentLoadOp loadOp;
    if (isFirstOffscreenPipeline) {
      loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    } else {
      loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    VkImageLayout initialLayout;
    if (isFirstOffscreenPipeline) {
      initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    } else {
      initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    offscreenColorAttachmentDescriptions[i] = (VkAttachmentDescription){
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = loadOp,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = initialLayout,
        .finalLayout = finalLayout,
    };
    offscreenColorAttachmentReferences[i] = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
  }

  if (pipelineInfo.useDepthAttachment) {
    *depthAttachmentDescription = (VkAttachmentDescription){
        .format = depthBufferImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    *depthAttachmentReference = (VkAttachmentReference){
        .attachment = idx++,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
  }

#undef ITERATE_INFO
}

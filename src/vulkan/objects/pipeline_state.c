#include "pipeline_state.h"
#include "pipeline.h"

void vulkan_pipeline_framebuffer_state_init(vulkan_pipeline_framebuffer_state *framebufferState,
                                            vulkan_pipeline *pipeline, uint32_t swapChainImageIdx) {
  framebufferState->pipeline = pipeline;

  /// HIRO offscreen image views instead of NULL (offscreen images stored in frame state)
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(framebufferState->pipeline);
  uint32_t framebufferAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  assert(framebufferAttachmentCount > 0);
  VkImageView framebufferAttachments[framebufferAttachmentCount];
  vulkan_pipeline_info_get_framebuffer_attachment_image_views(
      pipelineInfo,
      *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx), NULL,
      pipeline->pipelineSharedState->depthBufferImage->imageView, framebufferAttachments);

  framebufferState->framebuffer = vulkan_create_framebuffer(
      framebufferState->pipeline->vks->vkd, framebufferState->pipeline->renderPass,
      framebufferAttachmentCount, framebufferAttachments,
      framebufferState->pipeline->vks->swapChainExtent.width,
      framebufferState->pipeline->vks->swapChainExtent.height, "pipeline framebuffer state #%d",
      swapChainImageIdx);
}

void vulkan_pipeline_framebuffer_state_deinit(vulkan_pipeline_framebuffer_state *framebufferState) {
  vkDestroyFramebuffer(framebufferState->pipeline->vks->vkd->device, framebufferState->framebuffer,
                       vka);
}

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline) {
  frameState->pipeline = pipeline;
#define x(_name, ...)                                                                              \
  if (frameState->pipeline->type == vulkan_pipeline_type_##_name) {                                \
    vulkan_pipeline_impl_##_name##_frame_state_init(frameState);                                   \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState) {
#define x(_name, ...)                                                                              \
  if (frameState->pipeline->type == vulkan_pipeline_type_##_name) {                                \
    vulkan_pipeline_impl_##_name##_frame_state_deinit(frameState);                                 \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState) {
#define x(_name, ...)                                                                              \
  if (frameState->pipeline->type == vulkan_pipeline_type_##_name) {                                \
    vulkan_pipeline_impl_##_name##_frame_state_send_to_device(frameState);                         \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent) {
  log_debug(INDENT_FORMAT_STRING "pipeline frame state", INDENT_FORMAT_ARGS(0));
}

vulkan_pipeline_shared_state *vulkan_pipeline_shared_state_create(vulkan_swap_chain *vks) {
  vulkan_pipeline_shared_state *pipelineSharedState =
      core_alloc(sizeof(vulkan_pipeline_shared_state));

  vulkan_pipeline_shared_state_init(pipelineSharedState, vks);

  return pipelineSharedState;
}

void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_pipeline_shared_state_deinit(pipelineSharedState);
  core_free(pipelineSharedState);
}

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *pipelineSharedState,
                                       vulkan_swap_chain *vks) {
  pipelineSharedState->depthBufferImage =
      vulkan_image_create(vks->vkd, vulkan_image_type_depth_buffer, vks->swapChainExtent.width,
                          vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_image_destroy(pipelineSharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *pipelineSharedState,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_image_debug_print(pipelineSharedState->depthBufferImage, indent + 2);
}
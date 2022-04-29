#include "pipeline_state.h"
#include "pipeline.h"

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline, uint32_t swapChainImageIdx) {
  frameState->pipeline = pipeline;

  /// HIRO offscreen framebuffer
  size_t framebufferAttachmentCount =
      vulkan_pipeline_get_framebuffer_attachment_count(frameState->pipeline);
  assert(framebufferAttachmentCount > 0);
  VkImageView framebufferAttachments[framebufferAttachmentCount];
  vulkan_pipeline_get_framebuffer_attachments(frameState->pipeline, swapChainImageIdx,
                                              framebufferAttachments);

  frameState->framebuffer = vulkan_create_framebuffer(
      frameState->pipeline->vks->vkd, frameState->pipeline->renderPass, framebufferAttachmentCount,
      framebufferAttachments, frameState->pipeline->vks->swapChainExtent.width,
      frameState->pipeline->vks->swapChainExtent.height, "frame state #%d", swapChainImageIdx);

  vulkan_batches_data_init(&frameState->batchesData, frameState->pipeline->vks->vkd);
}

void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState) {
  vulkan_batches_data_deinit(&frameState->batchesData);

  vkDestroyFramebuffer(frameState->pipeline->vks->vkd->device, frameState->framebuffer, vka);
}

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState) {
  vulkan_batches_data_send_to_device(&frameState->batchesData);
}

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent) {
  log_debug(INDENT_FORMAT_STRING "frame state", INDENT_FORMAT_ARGS(0));
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

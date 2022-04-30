#include "pipeline.h"

void create_render_pass(vulkan_pipeline *pipeline) {
  uint32_t colorAttachmentCount;
  bool useDepthAttachment;
  vulkan_pipeline_get_framebuffer_attachment_count(pipeline, &(uint32_t){0}, &colorAttachmentCount,
                                                   &useDepthAttachment);
  assert(colorAttachmentCount > 0);

  VkAttachmentDescription colorAttachmentDescriptions[colorAttachmentCount];
  VkAttachmentReference colorAttachmentReferences[colorAttachmentCount];
  VkAttachmentDescription depthAttachmentDescription;
  VkAttachmentReference depthAttachmentReference;

#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_get_render_pass_create_info(                                    \
        pipeline, colorAttachmentDescriptions, colorAttachmentReferences,                          \
        &depthAttachmentDescription, &depthAttachmentReference);                                   \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x

  pipeline->renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, colorAttachmentDescriptions, array_size(colorAttachmentDescriptions),
      colorAttachmentReferences, array_size(colorAttachmentReferences),
      (useDepthAttachment ? &depthAttachmentDescription : NULL),
      (useDepthAttachment ? &depthAttachmentReference : NULL), "pipeline");
}

void create_graphics_pipeline(vulkan_pipeline *pipeline) {
  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  pipeline->graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd, pipeline->shaderProgram, pipeline->renderState, pipeline->vks,
      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, &pipeline->pipelineLayout, "pipeline");

  core_free(descriptorSetLayouts);
}

vulkan_pipeline *vulkan_pipeline_create(vulkan_pipeline_type type, vulkan_swap_chain *vks,
                                        vulkan_render_state *renderState,
                                        vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));
  vulkan_pipeline_init(pipeline, type, vks, renderState, pipelineSharedState);
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_pipeline_deinit(pipeline);
  core_free(pipeline);
}

void vulkan_pipeline_init(vulkan_pipeline *pipeline, vulkan_pipeline_type type,
                          vulkan_swap_chain *vks, vulkan_render_state *renderState,
                          vulkan_pipeline_shared_state *pipelineSharedState) {
  pipeline->vks = vks;
  pipeline->renderState = renderState;
  pipeline->pipelineSharedState = pipelineSharedState;

  pipeline->type = type;
  // HIRO HIRO type (different shader program)
  pipeline->shaderProgram = vulkan_shader_program_create(renderState);
  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);

  utarray_alloc(pipeline->frameStates, sizeof(vulkan_pipeline_frame_state));
  utarray_resize(pipeline->frameStates, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_init(frameState, pipeline, swapChainImageIdx);
    swapChainImageIdx++;
  }
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_deinit(frameState);
  }
  utarray_free(pipeline->frameStates);

  vkDestroyPipelineLayout(pipeline->vks->vkd->device, pipeline->pipelineLayout, vka);
  vkDestroyPipeline(pipeline->vks->vkd->device, pipeline->graphicsPipeline, vka);
  vkDestroyRenderPass(pipeline->vks->vkd->device, pipeline->renderPass, vka);
  vulkan_shader_program_destroy(pipeline->shaderProgram);
}

void vulkan_pipeline_get_framebuffer_attachment_count(vulkan_pipeline *pipeline,
                                                      uint32_t *attachmentCount,
                                                      uint32_t *colorAttachmentCount,
                                                      bool *useDepthAttachment) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_count(                               \
        pipeline, colorAttachmentCount, useDepthAttachment);                                       \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
  *attachmentCount = *colorAttachmentCount + (*useDepthAttachment ? 1 : 0);
}

void vulkan_pipeline_get_framebuffer_attachment_image_views(vulkan_pipeline *pipeline,
                                                            size_t swapChainImageIdx,
                                                            VkImageView *attachments) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_image_views(                         \
        pipeline, swapChainImageIdx, attachments);                                                 \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_get_framebuffer_attachment_clear_values(vulkan_pipeline *pipeline,
                                                             VkClearValue *clearValues) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_clear_values(pipeline, clearValues); \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline, size_t swapChainImageIdx) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, swapChainImageIdx);

  vulkan_pipeline_frame_state_send_to_device(frameState);
}

void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                        size_t swapChainImageIdx) {
  // TODO: Replace swapChainImageIdx with currentFrameIdx.
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_impl_##_name##_record_render_pass(pipeline, commandBuffer, swapChainImageIdx); \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("pipeline:\n");
  vulkan_shader_program_debug_print(pipeline->shaderProgram, 2);
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_debug_print(frameState, 2);
  }
  vulkan_pipeline_shared_state_debug_print(pipeline->pipelineSharedState, 2);
}

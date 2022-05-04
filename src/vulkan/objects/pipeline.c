#include "pipeline.h"

void create_render_pass(vulkan_pipeline *pipeline) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  bool useOnscreenColorAttachment = pipelineInfo.useOnscreenColorAttachment;
  uint32_t offscreenColorAttachmentCount = pipelineInfo.offscreenColorAttachmentCount;
  bool useDepthAttachment = pipelineInfo.useDepthAttachment;

  assert(useOnscreenColorAttachment || offscreenColorAttachmentCount > 0);

  VkAttachmentDescription onscreenColorAttachmentDescription;
  VkAttachmentReference onscreenColorAttachmentReference;
  VkAttachmentDescription offscreenColorAttachmentDescriptions[VLA(offscreenColorAttachmentCount)];
  VkAttachmentReference offscreenColorAttachmentReferences[VLA(offscreenColorAttachmentCount)];
  VkAttachmentDescription depthAttachmentDescription;
  VkAttachmentReference depthAttachmentReference;
  vulkan_pipeline_info_get_render_pass_create_info(
      pipelineInfo, pipeline->vks->swapChainImageFormat,
      vulkan_find_depth_format(pipeline->vks->vkd), &onscreenColorAttachmentDescription,
      &onscreenColorAttachmentReference, offscreenColorAttachmentDescriptions,
      offscreenColorAttachmentReferences, &depthAttachmentDescription, &depthAttachmentReference);

  pipeline->renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, (useOnscreenColorAttachment ? &onscreenColorAttachmentDescription : NULL),
      (useOnscreenColorAttachment ? &onscreenColorAttachmentReference : NULL),
      offscreenColorAttachmentDescriptions, offscreenColorAttachmentCount,
      offscreenColorAttachmentReferences, offscreenColorAttachmentCount,
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
  pipeline->shaderProgram = vulkan_shader_program_create(renderState, pipeline->type);
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

vulkan_pipeline_info vulkan_pipeline_get_pipeline_info(vulkan_pipeline *pipeline) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    return vulkan_pipeline_impl_##_name##_get_pipeline_info(pipeline);                             \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
  assert(0);
  return (vulkan_pipeline_info){0};
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

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
      pipelineInfo, pipeline->prev, pipeline->next, pipeline->vks->swapChainImageFormat,
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

  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_shader_program_get_shader_stages(pipeline->shaderProgram, &shaderStageCount);

  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  uint32_t vertexBindingDescriptionsCount =
      vulkan_vertex_stream_get_vertex_buffer_binding_count(pipeline->renderState->vertexStream);
  assert(vertexBindingDescriptionsCount == 1);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_vertex_stream_get_vertex_buffer_binding_description(
          pipeline->renderState->vertexStream);

  uint32_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_vertex_stream_get_vertex_attribute_descriptions(pipeline->renderState->vertexStream,
                                                             &vertexAttributeDescriptionsCount);

  pipeline->graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd,

      shaderStages, shaderStageCount,

      &vertexInputBindingDescription, vertexBindingDescriptionsCount, vertexAttributeDescriptions,
      vertexAttributeDescriptionsCount,

      pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,

      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, &pipeline->pipelineLayout, "pipeline");

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);
}

vulkan_pipeline *vulkan_pipeline_create_start(vulkan_pipeline_type type, vulkan_swap_chain *vks,
                                              vulkan_render_state *renderState,
                                              vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));
  vulkan_pipeline_init_start(pipeline, type, vks, renderState, pipelineSharedState);
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_pipeline_deinit(pipeline);
  core_free(pipeline);
}

void vulkan_pipeline_init_start(vulkan_pipeline *pipeline, vulkan_pipeline_type type,
                                vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                vulkan_pipeline_shared_state *pipelineSharedState) {
  pipeline->vks = vks;
  pipeline->renderState = renderState;
  pipeline->pipelineSharedState = pipelineSharedState;

  pipeline->type = type;
  pipeline->shaderProgram = vulkan_shader_program_create(renderState, pipeline->type);
}

void vulkan_pipeline_init_prev_next(vulkan_pipeline *pipeline, vulkan_pipeline *prev,
                                    vulkan_pipeline *next) {
  pipeline->prev = prev;
  pipeline->next = next;
}

void vulkan_pipeline_init_finish(vulkan_pipeline *pipeline) {

  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);

  utarray_alloc(pipeline->framebufferStates, sizeof(vulkan_pipeline_frame_state));
  utarray_resize(pipeline->framebufferStates, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (vulkan_pipeline_framebuffer_state *, framebufferStates,
                           pipeline->framebufferStates) {
    vulkan_pipeline_framebuffer_state_init(framebufferStates, pipeline, swapChainImageIdx);
    swapChainImageIdx++;
  }

  utarray_alloc(pipeline->frameStates, sizeof(vulkan_pipeline_frame_state));
  utarray_resize(pipeline->frameStates, FRAMES_IN_FLIGHT);
  utarray_foreach_elem_it (vulkan_pipeline_frame_state *, frameState, pipeline->frameStates) {
    vulkan_pipeline_frame_state_init(frameState, pipeline);
  }
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_it (vulkan_pipeline_framebuffer_state *, framebufferStates,
                           pipeline->framebufferStates) {
    vulkan_pipeline_framebuffer_state_deinit(framebufferStates);
  }
  utarray_free(pipeline->framebufferStates);

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

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline) {
  vulkan_pipeline_frame_state *frameState =
      utarray_eltptr(pipeline->frameStates, pipeline->renderState->sync->currentFrameInFlight);

  vulkan_pipeline_frame_state_send_to_device(frameState);
}

void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                        size_t swapChainImageIdx) {
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

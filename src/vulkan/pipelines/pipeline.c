#include "pipeline.h"

void create_render_pass(vulkan_pipeline *pipeline) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);
  bool useOnscreenColorAttachment = pipelineInfo.useOnscreenColorAttachment;
  uint32_t offscreenColorAttachmentCount = pipelineInfo.offscreenColorAttachmentCount;
  bool useDepthAttachment = pipelineInfo.useDepthAttachment;
  assert(useOnscreenColorAttachment || offscreenColorAttachmentCount > 0);

  VkFormat swapChainImageFormat = pipeline->vks->swapChainImageFormat;
  VkFormat offscreenFormats[VLA(offscreenColorAttachmentCount)];
  vulkan_pipeline_info_get_offscreen_framebuffer_attachment_formats(pipelineInfo, pipeline,
                                                                    offscreenFormats);
  VkFormat depthBufferImageFormat = vulkan_find_depth_format(pipeline->vks->vkd);

  VkAttachmentDescription onscreenColorAttachmentDescription;
  VkAttachmentReference onscreenColorAttachmentReference;
  VkAttachmentDescription offscreenColorAttachmentDescriptions[VLA(offscreenColorAttachmentCount)];
  VkAttachmentReference offscreenColorAttachmentReferences[VLA(offscreenColorAttachmentCount)];
  VkAttachmentDescription depthAttachmentDescription;
  VkAttachmentReference depthAttachmentReference;
  vulkan_pipeline_info_get_render_pass_create_info(
      pipelineInfo, pipeline->prev, pipeline->next, swapChainImageFormat, offscreenFormats,
      depthBufferImageFormat, &onscreenColorAttachmentDescription,
      &onscreenColorAttachmentReference, offscreenColorAttachmentDescriptions,
      offscreenColorAttachmentReferences, &depthAttachmentDescription, &depthAttachmentReference);

  pipeline->renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, (useOnscreenColorAttachment ? &onscreenColorAttachmentDescription : NULL),
      (useOnscreenColorAttachment ? &onscreenColorAttachmentReference : NULL),
      offscreenColorAttachmentDescriptions, offscreenColorAttachmentCount,
      offscreenColorAttachmentReferences, offscreenColorAttachmentCount,
      (useDepthAttachment ? &depthAttachmentDescription : NULL),
      (useDepthAttachment ? &depthAttachmentReference : NULL), "pipeline %s",
      vulkan_pipeline_type_debug_str(pipeline->type));
}

void create_graphics_pipeline(vulkan_pipeline *pipeline) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);

  uint32_t colorAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_color_attachment_count(pipelineInfo);
  bool colorBlendingType = pipelineInfo.colorBlendingType;

  bool depthWriteEnable =
      pipelineInfo.useDepthAttachment && pipelineInfo.depthAttachmentWriteEnable;
  bool depthTestEnable = pipelineInfo.useDepthAttachment && pipelineInfo.depthAttachmentTestEnable;
  VkCompareOp depthTestOp = pipelineInfo.depthAttachmentTestOp;

  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_pipeline_shader_program_get_shader_stages(pipeline->shaderProgram, &shaderStageCount);

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

      colorAttachmentCount, colorBlendingType,

      depthWriteEnable, depthTestEnable, depthTestOp,

      shaderStages, shaderStageCount,

      &vertexInputBindingDescription, vertexBindingDescriptionsCount, vertexAttributeDescriptions,
      vertexAttributeDescriptionsCount,

      pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,

      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, pipeline->renderState->descriptors->pipelineLayout, "pipeline %s",
      vulkan_pipeline_type_debug_str(pipeline->type));

  core_free(vertexAttributeDescriptions);
  core_free(descriptorSetLayouts);
}

void create_framebuffers(vulkan_pipeline *pipeline) {
  vulkan_pipeline_info pipelineInfo = vulkan_pipeline_get_pipeline_info(pipeline);

  uint32_t framebufferAttachmentCount =
      vulkan_pipeline_info_get_framebuffer_attachment_count(pipelineInfo);
  assert(framebufferAttachmentCount > 0);
  uint32_t offscreenFramebufferAttachmentCount = pipelineInfo.offscreenColorAttachmentCount;

  utarray_alloc(pipeline->framebuffers, sizeof(VkFramebuffer));
  utarray_resize(pipeline->framebuffers, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (VkFramebuffer *, framebuffer, pipeline->framebuffers) {
    VkImageView swapChainImageView =
        *(VkImageView *)utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx);
    VkImageView depthBufferImageView =
        pipeline->pipelineState->sharedState.depthBufferImage->imageView;
    VkImageView offscreenImageViews[VLA(offscreenFramebufferAttachmentCount)];
    vulkan_pipeline_info_get_offscreen_framebuffer_attachment_image_views(pipelineInfo, pipeline,
                                                                          offscreenImageViews);

    VkImageView framebufferAttachments[framebufferAttachmentCount];
    vulkan_pipeline_info_get_framebuffer_attachment_image_views(
        pipelineInfo, swapChainImageView, offscreenImageViews, depthBufferImageView,
        framebufferAttachments);

    *framebuffer = vulkan_create_framebuffer(
        pipeline->vks->vkd, pipeline->renderPass, framebufferAttachmentCount,
        framebufferAttachments, pipeline->vks->swapChainExtent.width,
        pipeline->vks->swapChainExtent.height, "pipeline framebuffer state #%d", swapChainImageIdx);

    swapChainImageIdx++;
  }
}

vulkan_pipeline *vulkan_pipeline_create_start(vulkan_pipeline_type type, vulkan_swap_chain *vks,
                                              vulkan_render_state *renderState,
                                              vulkan_pipeline_state *pipelineState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));
  vulkan_pipeline_init_start(pipeline, type, vks, renderState, pipelineState);
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_pipeline_deinit(pipeline);
  core_free(pipeline);
}

void vulkan_pipeline_init_start(vulkan_pipeline *pipeline, vulkan_pipeline_type type,
                                vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                vulkan_pipeline_state *pipelineState) {
  pipeline->vks = vks;
  pipeline->renderState = renderState;
  pipeline->pipelineState = pipelineState;

  pipeline->type = type;
  pipeline->shaderProgram = vulkan_pipeline_shader_program_create(
      renderState, vulkan_pipeline_get_pipeline_info(pipeline));
}

void vulkan_pipeline_init_prev_next(vulkan_pipeline *pipeline, vulkan_pipeline *prev,
                                    vulkan_pipeline *next) {
  pipeline->prev = prev;
  pipeline->next = next;
}

void vulkan_pipeline_init_finish(vulkan_pipeline *pipeline) {
  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);
  create_framebuffers(pipeline);
}

void vulkan_pipeline_deinit(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_deref (VkFramebuffer, framebuffer, pipeline->framebuffers) {
    vkDestroyFramebuffer(pipeline->vks->vkd->device, framebuffer, vka);
  }
  utarray_free(pipeline->framebuffers);

  vkDestroyPipeline(pipeline->vks->vkd->device, pipeline->graphicsPipeline, vka);
  vkDestroyRenderPass(pipeline->vks->vkd->device, pipeline->renderPass, vka);
  vulkan_pipeline_shader_program_destroy(pipeline->shaderProgram);
}

vulkan_pipeline_info vulkan_pipeline_get_pipeline_info(vulkan_pipeline *pipeline) {
#define x(_name, ...)                                                                              \
  if (pipeline->type == vulkan_pipeline_type_##_name) {                                            \
    vulkan_pipeline_info pipelineInfo =                                                            \
        vulkan_pipeline_impl_##_name##_get_pipeline_info(pipeline);                                \
    pipelineInfo.pipelineType = pipeline->type;                                                    \
    return pipelineInfo;                                                                           \
  }
  VULKAN_PIPELINE_TYPES(x, )
#undef x
  assert(0);
  return (vulkan_pipeline_info){0};
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
  vulkan_pipeline_shader_program_debug_print(pipeline->shaderProgram, 2);
  vulkan_pipeline_state_debug_print(pipeline->pipelineState, 2);
}

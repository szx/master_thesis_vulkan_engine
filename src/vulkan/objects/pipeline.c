#include "pipeline.h"

void create_framebuffers(vulkan_pipeline *pipeline) {
  utarray_alloc(pipeline->swapChainFramebuffers, sizeof(VkFramebuffer));
  utarray_resize(pipeline->swapChainFramebuffers, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (VkFramebuffer *, swapChainFramebuffer, pipeline->swapChainFramebuffers) {
    *swapChainFramebuffer = vulkan_create_framebuffer(
        pipeline->vks->vkd, pipeline->renderPass, 1,
        utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx),
        pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,
        "pipeline (swap chain image #%d)", swapChainImageIdx);
    swapChainImageIdx++;
  }
}

void destroy_framebuffers(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_deref (VkFramebuffer, swapChainFramebuffer,
                              pipeline->swapChainFramebuffers) {
    vkDestroyFramebuffer(pipeline->vks->vkd->device, swapChainFramebuffer, vka);
  }
  utarray_free(pipeline->swapChainFramebuffers);
}

void create_render_pass(vulkan_pipeline *pipeline) {
  VkAttachmentDescription colorAttachment = {0};
  colorAttachment.format = pipeline->vks->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {0};
  depthAttachment.format = vulkan_find_depth_format(pipeline->vks->vkd);
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {0};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {0};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentDescriptions[] = {colorAttachment};
  VkAttachmentReference colorAttachmentReferences[] = {colorAttachmentRef};

  pipeline->renderPass = vulkan_create_render_pass(
      pipeline->vks->vkd, colorAttachmentDescriptions, array_size(colorAttachmentDescriptions),
      colorAttachmentReferences, array_size(colorAttachmentReferences), depthAttachment,
      depthAttachmentRef, "pipeline");
}

void destroy_render_pass(vulkan_pipeline *pipeline) {
  vkDestroyRenderPass(pipeline->vks->vkd->device, pipeline->renderPass, vka);
}

void create_graphics_pipeline(vulkan_pipeline *pipeline) {
  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->unifiedUniformBuffer->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 0;
  const VkPushConstantRange *pushConstantRanges = NULL;

  pipeline->graphicsPipeline = vulkan_create_graphics_pipeline(
      pipeline->vks->vkd, pipeline->shaderGenerator, pipeline->renderState, pipeline->vks,
      descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges, pushConstantRangeCount,
      pipeline->renderPass, &pipeline->pipelineLayout, "pipeline");

  core_free(descriptorSetLayouts);
}

void destroy_graphics_pipeline(vulkan_pipeline *pipeline) {
  vkDestroyPipelineLayout(pipeline->vks->vkd->device, pipeline->pipelineLayout, vka);
  vkDestroyPipeline(pipeline->vks->vkd->device, pipeline->graphicsPipeline, vka);
}

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  pipeline->vks = vks;
  pipeline->renderState = renderState;

  pipeline->shaderGenerator = vulkan_shader_generator_create(renderState);

  create_render_pass(pipeline);
  create_graphics_pipeline(pipeline);
  create_framebuffers(pipeline);

  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  destroy_framebuffers(pipeline);
  destroy_graphics_pipeline(pipeline);
  destroy_render_pass(pipeline);
  vulkan_shader_generator_destroy(pipeline->shaderGenerator);
  core_free(pipeline);
}

void vulkan_pipeline_record_command_buffer(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                           size_t inFlightImage) {
  // HIRO: Record command buffer.
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("PIPELINE:\n");
  vulkan_shader_generator_debug_print(pipeline->shaderGenerator, 2);
}

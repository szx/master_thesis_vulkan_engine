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

void create_pipeline_layout(vulkan_pipeline *pipeline) {
  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts = vulkan_descriptors_get_descriptor_set_layouts(
      pipeline->renderState->unifiedUniformBuffer->descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 0;
  const VkPushConstantRange *pushConstantRanges = NULL;

  pipeline->pipelineLayout = vulkan_create_pipeline_layout(
      pipeline->vks->vkd, 0, descriptorSetLayouts, descriptorSetLayoutCount, pushConstantRanges,
      pushConstantRangeCount, "pipeline");

  core_free(descriptorSetLayouts);
}

void destroy_pipeline_layout(vulkan_pipeline *pipeline) {
  vkDestroyPipelineLayout(pipeline->vks->vkd->device, pipeline->pipelineLayout, vka);
}

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  pipeline->vks = vks;
  pipeline->renderState = renderState;

  pipeline->shaderGenerator = vulkan_shader_generator_create(renderState);

  // HIRO HIRO create pipeline layout (move to descriptors?)
  // HIRO create render pass and graphics pipeline (use input desc struct?)
  pipeline->renderPass = VK_NULL_HANDLE;

  create_pipeline_layout(pipeline);
  create_framebuffers(pipeline);

  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  destroy_framebuffers(pipeline);
  destroy_pipeline_layout(pipeline);
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

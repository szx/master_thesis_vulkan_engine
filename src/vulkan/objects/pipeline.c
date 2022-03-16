#include "pipeline.h"

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  pipeline->vks = vks;
  pipeline->renderState = renderState;

  pipeline->shaderGenerator = vulkan_shader_generator_create(renderState);

  // HIRO HIRO create pipeline layout (move to descriptors?)
  // HIRO create render pass and graphics pipeline (use input desc struct?)
  pipeline->renderPass = VK_NULL_HANDLE;

  utarray_alloc(pipeline->swapChainFramebuffers, sizeof(VkFramebuffer));
  utarray_resize(pipeline->swapChainFramebuffers, utarray_len(pipeline->vks->swapChainImageViews));
  size_t swapChainImageIdx = 0;
  utarray_foreach_elem_it (VkFramebuffer *, swapChainFramebuffer, pipeline->swapChainFramebuffers) {
    *swapChainFramebuffer = vulkan_create_framebuffer(
        renderState->vkd, pipeline->renderPass, 1,
        utarray_eltptr(pipeline->vks->swapChainImageViews, swapChainImageIdx),
        pipeline->vks->swapChainExtent.width, pipeline->vks->swapChainExtent.height,
        "framebuffer #%d", swapChainImageIdx);
    swapChainImageIdx++;
  }

  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_deref (VkFramebuffer, swapChainFramebuffer,
                              pipeline->swapChainFramebuffers) {
    vkDestroyFramebuffer(pipeline->vks->vkd->device, swapChainFramebuffer, vka);
  }
  utarray_free(pipeline->swapChainFramebuffers);

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

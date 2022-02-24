#include "pipeline.h"

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  pipeline->shaderGenerator = vulkan_shader_generator_create(renderState);

  // HIRO create descriptor sets (move to render state)?
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  vulkan_shader_generator_destroy(pipeline->shaderGenerator);
  core_free(pipeline);
}

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline) {
  // HIRO: Update descriptor sets (move to vulkan_render_state_update?)
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("PIPELINE:\n");
  vulkan_shader_generator_debug_print(pipeline->shaderGenerator, 2);
}

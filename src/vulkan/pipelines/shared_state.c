#include "shared_state.h"

vulkan_pipeline_shared_state *
vulkan_pipeline_shared_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_shared_state *pipelineSharedState =
      core_alloc(sizeof(vulkan_pipeline_shared_state));

  vulkan_pipeline_shared_state_init(pipelineSharedState, renderState);

  return pipelineSharedState;
}

void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_pipeline_shared_state_deinit(pipelineSharedState);
  core_free(pipelineSharedState);
}

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *pipelineSharedState,
                                       vulkan_render_state *renderState) {
  pipelineSharedState->renderState = renderState;

  pipelineSharedState->camera =
      vulkan_pipeline_camera_state_create(pipelineSharedState->renderState);
  pipelineSharedState->lights =
      vulkan_pipeline_light_state_create(pipelineSharedState->renderState);
  pipelineSharedState->skybox =
      vulkan_pipeline_skybox_state_create(pipelineSharedState->renderState);
  pipelineSharedState->renderCacheListBatches = vulkan_batches_create(
      pipelineSharedState->renderState->renderCacheList, pipelineSharedState->renderState->vkd);

  pipelineSharedState->depthBufferImage = vulkan_image_create(
      pipelineSharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      pipelineSharedState->renderState->vks->swapChainExtent.width,
      pipelineSharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *pipelineSharedState) {
  vulkan_batches_destroy(pipelineSharedState->renderCacheListBatches);
  vulkan_pipeline_skybox_state_destroy(pipelineSharedState->skybox);
  vulkan_pipeline_light_state_destroy(pipelineSharedState->lights);
  vulkan_pipeline_camera_state_destroy(pipelineSharedState->camera);

  vulkan_image_destroy(pipelineSharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_reinit_with_new_swap_chain(
    vulkan_pipeline_shared_state *pipelineSharedState) {

  vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(pipelineSharedState->skybox);
  vulkan_pipeline_light_state_reinit_with_new_swap_chain(pipelineSharedState->lights);
  vulkan_pipeline_camera_state_reinit_with_new_swap_chain(pipelineSharedState->camera);

  vulkan_image_destroy(pipelineSharedState->depthBufferImage);
  pipelineSharedState->depthBufferImage = vulkan_image_create(
      pipelineSharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      pipelineSharedState->renderState->vks->swapChainExtent.width,
      pipelineSharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_update(vulkan_pipeline_shared_state *pipelineSharedState,
                                         vulkan_batch_instancing_policy instancingPolicy) {
  vulkan_pipeline_camera_state_update(pipelineSharedState->camera);
  vulkan_pipeline_light_state_update(pipelineSharedState->lights);
  vulkan_pipeline_skybox_state_update(pipelineSharedState->skybox);

  vulkan_batches_update(pipelineSharedState->renderCacheListBatches, instancingPolicy);
  dl_foreach_elem(vulkan_batch *, batch, pipelineSharedState->renderCacheListBatches->batches) {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  }
}

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *pipelineSharedState,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_batches_debug_print(pipelineSharedState->renderCacheListBatches);
  vulkan_pipeline_camera_state_debug_print(pipelineSharedState->camera, indent + 2);
  vulkan_pipeline_light_state_debug_print(pipelineSharedState->lights, indent + 2);
  vulkan_pipeline_skybox_state_debug_print(pipelineSharedState->skybox, indent + 2);
  vulkan_image_debug_print(pipelineSharedState->depthBufferImage, indent + 2);
}

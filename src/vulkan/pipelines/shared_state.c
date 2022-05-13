#include "shared_state.h"

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *sharedState,
                                       vulkan_render_state *renderState) {
  sharedState->renderState = renderState;

  sharedState->camera = vulkan_pipeline_camera_state_create(sharedState->renderState);
  sharedState->lights = vulkan_pipeline_light_state_create(sharedState->renderState);
  sharedState->skybox = vulkan_pipeline_skybox_state_create(sharedState->renderState);
  sharedState->rendererCacheBatches =
      vulkan_batches_create(sharedState->renderState->rendererCache, sharedState->renderState->vkd);

  sharedState->depthBufferImage = vulkan_image_create(
      sharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      sharedState->renderState->vks->swapChainExtent.width,
      sharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *sharedState) {
  vulkan_batches_destroy(sharedState->rendererCacheBatches);
  vulkan_pipeline_skybox_state_destroy(sharedState->skybox);
  vulkan_pipeline_light_state_destroy(sharedState->lights);
  vulkan_pipeline_camera_state_destroy(sharedState->camera);

  vulkan_image_destroy(sharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_reinit_with_new_swap_chain(
    vulkan_pipeline_shared_state *sharedState) {

  vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(sharedState->skybox);
  vulkan_pipeline_light_state_reinit_with_new_swap_chain(sharedState->lights);
  vulkan_pipeline_camera_state_reinit_with_new_swap_chain(sharedState->camera);

  vulkan_image_destroy(sharedState->depthBufferImage);
  sharedState->depthBufferImage = vulkan_image_create(
      sharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      sharedState->renderState->vks->swapChainExtent.width,
      sharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_update(vulkan_pipeline_shared_state *sharedState) {
  log_debug("updating pipeline shared state");

  vulkan_pipeline_camera_state_update(sharedState->camera);
  vulkan_pipeline_light_state_update(sharedState->lights);
  vulkan_pipeline_skybox_state_update(sharedState->skybox);

  vulkan_draw_call_instancing_policy instancingPolicy =
      (sharedState->renderState->config->asset.graphicsEnabledInstancing
           ? vulkan_draw_call_instancing_policy_matching_vertex_attributes
           : vulkan_draw_call_instancing_policy_no_instancing);
  vulkan_batches_update(sharedState->rendererCacheBatches, instancingPolicy);
  dl_foreach_elem(vulkan_draw_call *, batch, sharedState->rendererCacheBatches->batches) {
    vulkan_draw_call_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  }
}

void vulkan_pipeline_shared_state_send_to_device(vulkan_pipeline_shared_state *sharedState) {
  vulkan_image_send_to_device(sharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *sharedState,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_batches_debug_print(sharedState->rendererCacheBatches);
  vulkan_pipeline_camera_state_debug_print(sharedState->camera, indent + 2);
  vulkan_pipeline_light_state_debug_print(sharedState->lights, indent + 2);
  vulkan_pipeline_skybox_state_debug_print(sharedState->skybox, indent + 2);
  vulkan_image_debug_print(sharedState->depthBufferImage, indent + 2);
}

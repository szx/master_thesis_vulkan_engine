#include "renderer.h"

vulkan_renderer *vulkan_renderer_create(data_config *config, data_asset_db *assetDb,
                                        vulkan_swap_chain *vks, UT_string *sceneName) {
  vulkan_renderer *renderer = core_alloc(sizeof(vulkan_renderer));

  renderer->config = config;
  renderer->assetDb = assetDb;
  renderer->data = vulkan_data_scene_create_with_asset_db(renderer->assetDb, sceneName);

  renderer->renderCacheList =
      vulkan_render_cache_list_create(renderer->config->graphicsMaxInstanceCount);

  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data, renderer->renderCacheList);

  renderer->vkd = vks->vkd;
  renderer->vks = vks;
  renderer->renderState = vulkan_render_state_create(renderer->vkd, renderer->renderCacheList);
  renderer->pipeline = vulkan_pipeline_create(renderer->vks, renderer->renderState);

  return renderer;
}

void vulkan_renderer_destroy(vulkan_renderer *renderer) {
  vulkan_pipeline_destroy(renderer->pipeline);
  vulkan_render_state_destroy(renderer->renderState);

  vulkan_scene_graph_destroy(renderer->sceneGraph);
  vulkan_render_cache_list_destroy(renderer->renderCacheList);
  vulkan_data_scene_destroy(renderer->data);

  core_free(renderer);
}

void vulkan_renderer_recreate_swap_chain(vulkan_renderer *renderer) {
  int width = 0, height = 0;
  glfwGetFramebufferSize(renderer->vkd->window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(renderer->vkd->window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(renderer->vkd->device);

  vulkan_pipeline_deinit(renderer->pipeline);

  vulkan_swap_chain_deinit(renderer->vks);

  vulkan_swap_chain_init(renderer->vks, renderer->vkd);

  vulkan_pipeline_init(renderer->pipeline, renderer->vks, renderer->renderState);
}

void vulkan_renderer_update(vulkan_renderer *renderer) {
  vulkan_scene_render_state_update(renderer->renderState);
}

void vulkan_renderer_send_to_device(vulkan_renderer *renderer) {
  vulkan_render_state_send_to_device(renderer->renderState);
}

void vulkan_renderer_draw_frame(vulkan_renderer *renderer) {
  vulkan_sync_wait_for_current_frame_fence(renderer->renderState->sync);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      renderer->vkd->device, renderer->vks->swapChain, UINT64_MAX,
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight],
      VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vulkan_renderer_recreate_swap_chain(renderer);
    return;
  }
  verify(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

  // Pre-submit CPU work:
  // We have acquired index of next in-flight image, we can now update frame-specific resources
  // (uniform buffers, push constants).
  // log_debug("imageIndex = %d", imageIndex);
  VkCommandBuffer commandBuffer =
      vulkan_pipeline_record_command_buffer(renderer->pipeline, imageIndex);

  // scene.updateScene(currentFrameInFlight);
  // scene.beginCommandBuffer(&framebuffers[imageIndex]);
  // scene.recordCommandBuffer(currentFrameInFlight, &framebuffers[imageIndex]);
  // gui.updateGUI(&scene);
  // gui.recordCommandBuffer(&framebuffers[imageIndex]);
  // scene.endCommandBuffer(&framebuffers[imageIndex]);

  vulkan_sync_reset_current_frame_fence(renderer->renderState->sync);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight]};
  // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
  // VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  VkSemaphore signalSemaphores[] = {
      renderer->renderState->sync
          ->renderFinishedSemaphores[renderer->renderState->sync->currentFrameInFlight]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  verify(vkQueueSubmit(renderer->vkd->graphicsQueue, 1, &submitInfo,
                       renderer->renderState->sync
                           ->inFlightFences[renderer->renderState->sync->currentFrameInFlight]) ==
         VK_SUCCESS);

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {renderer->vks->swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  result = vkQueuePresentKHR(renderer->vkd->presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      renderer->vkd->framebufferResized) {
    renderer->vkd->framebufferResized = false;
    vulkan_renderer_recreate_swap_chain(renderer);
  } else {
    verify(result == VK_SUCCESS);
  }
  vulkan_sync_advance_to_next_frame(renderer->renderState->sync);
}

void vulkan_renderer_run_main_loop(vulkan_renderer *renderer,
                                   void (*updateFunc)(vulkan_renderer *renderer, double dt)) {
  glfwMakeContextCurrent(renderer->vkd->window);

  double currentTime = glfwGetTime();
  while (glfwWindowShouldClose(renderer->vkd->window) == 0) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;
    while (frameTime > 0.0) {
      double dt = MIN(frameTime, MIN_DELTA_TIME);
      updateFunc(renderer, dt);
      vulkan_input_clear_released_keys(&renderer->vkd->input);
      frameTime -= dt;
    }
    glfwPollEvents(); // calls GLFW callbacks
    vulkan_renderer_update(renderer);
    vulkan_renderer_send_to_device(renderer);
    vulkan_renderer_draw_frame(renderer);
  }
  vkDeviceWaitIdle(renderer->vkd->device);
}

void vulkan_renderer_debug_print(vulkan_renderer *renderer) {
  log_debug("renderer:\n");
  vulkan_data_scene_debug_print(renderer->data);
  vulkan_render_cache_list_debug_print(renderer->renderCacheList);
  vulkan_scene_graph_debug_print(renderer->sceneGraph);
  vulkan_render_state_debug_print(renderer->renderState);
  vulkan_pipeline_debug_print(renderer->pipeline);
}

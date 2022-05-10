#include "renderer.h"

vulkan_renderer *vulkan_renderer_create(data_config *config, data_asset_db *assetDb,
                                        vulkan_swap_chain *vks, UT_string *sceneName,
                                        vulkan_pipeline_type *pipelines, size_t pipelineCount) {
  vulkan_renderer *renderer = core_alloc(sizeof(vulkan_renderer));

  renderer->config = config;
  renderer->assetDb = assetDb;
  renderer->data = vulkan_scene_data_create_with_asset_db(renderer->assetDb, sceneName);
  renderer->vkd = vks->vkd;
  renderer->vks = vks;

  renderer->renderCacheList =
      vulkan_render_cache_list_create(renderer->config->asset.graphicsMaxPrimitiveRenderCacheCount);

  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data, renderer->renderCacheList);

  renderer->renderState =
      vulkan_render_state_create(renderer->vks, renderer->renderCacheList, renderer->config);

  renderer->pipelineSharedState = vulkan_pipeline_shared_state_create(renderer->vks);

  utarray_alloc(renderer->pipelines, sizeof(vulkan_pipeline *));

  for (size_t i = 0; i < pipelineCount; i++) {
    vulkan_pipeline_type type = pipelines[i];
    vulkan_pipeline *pipeline = vulkan_pipeline_create_start(
        type, renderer->vks, renderer->renderState, renderer->pipelineSharedState);
    utarray_push_back(renderer->pipelines, &pipeline);
  }
  // HIRO GUI pipeline
  // HIRO screen-space postprocessing effects pipeline

  for (size_t i = 0; i < utarray_len(renderer->pipelines); i++) {
    vulkan_pipeline *pipeline = *(vulkan_pipeline **)utarray_eltptr(renderer->pipelines, i);
    vulkan_pipeline *prevPipeline = NULL;
    if (i > 0) {
      prevPipeline = *(vulkan_pipeline **)utarray_eltptr(renderer->pipelines, i - 1);
    }
    vulkan_pipeline *nextPipeline = NULL;
    if (i + 1 < utarray_len(renderer->pipelines)) {
      nextPipeline = *(vulkan_pipeline **)utarray_eltptr(renderer->pipelines, i + 1);
    }
    vulkan_pipeline_init_prev_next(pipeline, prevPipeline, nextPipeline);
  }

  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_init_finish(pipeline);
  }
  return renderer;
}

void vulkan_renderer_destroy(vulkan_renderer *renderer) {
  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_destroy(pipeline);
  }
  utarray_free(renderer->pipelines);

  vulkan_pipeline_shared_state_destroy(renderer->pipelineSharedState);

  vulkan_render_state_destroy(renderer->renderState);

  vulkan_scene_graph_destroy(renderer->sceneGraph);
  vulkan_render_cache_list_destroy(renderer->renderCacheList);
  vulkan_scene_data_destroy(renderer->data);

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

  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_deinit(pipeline);
  }

  vulkan_pipeline_shared_state_deinit(renderer->pipelineSharedState);

  vulkan_swap_chain_deinit(renderer->vks);

  vulkan_swap_chain_init(renderer->vks, renderer->vkd);

  vulkan_pipeline_shared_state_init(renderer->pipelineSharedState, renderer->vks);

  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_init_start(pipeline, pipeline->type, renderer->vks, renderer->renderState,
                               renderer->pipelineSharedState);
    vulkan_pipeline_init_prev_next(pipeline, pipeline->prev, pipeline->next);
    vulkan_pipeline_init_finish(pipeline);
  }
}

void vulkan_renderer_update(vulkan_renderer *renderer) {
  vulkan_render_state_update(renderer->renderState);
}

void vulkan_renderer_send_to_device(vulkan_renderer *renderer) {
  vulkan_render_state_send_to_device(renderer->renderState);
  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_send_to_device(pipeline);
  }
}

void vulkan_renderer_draw_frame(vulkan_renderer *renderer) {
  /* proceed to next frame */
  vulkan_sync_advance_to_next_frame(renderer->renderState->sync);
  vulkan_sync_wait_for_current_frame_fence(renderer->renderState->sync);

  /* acquire swap chain image */
  uint32_t swapChainImageIdx;
  VkResult result = vkAcquireNextImageKHR(
      renderer->vkd->device, renderer->vks->swapChain, UINT64_MAX,
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight],
      VK_NULL_HANDLE, &swapChainImageIdx);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vulkan_renderer_recreate_swap_chain(renderer);
    return;
  }
  verify(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

  /* Pre-submit CPU work:
   * We have acquired index of next in-flight image, we can now update
   * frame-specific resources (uniform buffers, push constants). */
  vulkan_renderer_update(renderer);
  vulkan_renderer_send_to_device(renderer);

  /* record command buffer */
  VkCommandBuffer commandBuffer =
      vulkan_sync_get_current_frame_command_buffer(renderer->renderState->sync);
  vkResetCommandBuffer(commandBuffer, 0);

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  verify(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);

  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_record_render_pass(pipeline, commandBuffer, swapChainImageIdx);
  }

  // scene.updateScene(currentFrameInFlight);
  // scene.beginCommandBuffer(&framebuffers[swapChainImageIdx]);
  // scene.recordCommandBuffer(currentFrameInFlight, &framebuffers[swapChainImageIdx]);
  // gui.updateGUI(&scene);
  // gui.recordCommandBuffer(&framebuffers[swapChainImageIdx]);
  // scene.endCommandBuffer(&framebuffers[swapChainImageIdx]);

  verify(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);

  /* submit command buffer */
  vulkan_sync_reset_current_frame_fence(renderer->renderState->sync);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight]};
  // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
  // VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
  presentInfo.pImageIndices = &swapChainImageIdx;
  result = vkQueuePresentKHR(renderer->vkd->presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      renderer->vkd->framebufferResized) {
    renderer->vkd->framebufferResized = false;
    vulkan_renderer_recreate_swap_chain(renderer);
  } else {
    verify(result == VK_SUCCESS);
  }
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
    vulkan_renderer_draw_frame(renderer);
  }
  vkDeviceWaitIdle(renderer->vkd->device);
}

void vulkan_renderer_debug_print(vulkan_renderer *renderer) {
  log_debug("renderer:\n");
  vulkan_scene_data_debug_print(renderer->data, 0);
  vulkan_render_cache_list_debug_print(renderer->renderCacheList);
  vulkan_scene_graph_debug_print(renderer->sceneGraph);
  vulkan_render_state_debug_print(renderer->renderState);
  utarray_foreach_elem_deref (vulkan_pipeline *, pipeline, renderer->pipelines) {
    vulkan_pipeline_debug_print(pipeline);
  }
}

#include "renderer.h"

renderer *renderer_create(data_config *config, data_asset_db *assetDb, swap_chain *vks,
                          UT_string *sceneName) {
  renderer *renderer = core_alloc(sizeof(struct renderer));

  renderer->config = config;
  renderer->assetDb = assetDb;
  renderer->data = scene_data_create_with_asset_db(config, renderer->assetDb, sceneName);
  renderer->vkd = vks->vkd;
  renderer->vks = vks;

  renderer->rendererCache = renderer_cache_create(
      renderer->data, renderer->config->global.graphicsMaxPrimitiveElementCount);

  renderer->sceneGraph = scene_graph_create(renderer->data, renderer->rendererCache);

  renderer->renderState =
      render_state_create(renderer->vks, renderer->rendererCache, renderer->config,
                          renderer_update_unified_uniform_buffer_callback);

  renderer->renderGraph = render_graph_create(renderer->renderState);

  return renderer;
}

void renderer_destroy(renderer *renderer) {
  render_graph_destroy(renderer->renderGraph);
  render_state_destroy(renderer->renderState);

  scene_graph_destroy(renderer->sceneGraph);
  renderer_cache_destroy(renderer->rendererCache);
  scene_data_destroy(renderer->data);

  core_free(renderer);
}

void renderer_recreate_swap_chain(renderer *renderer) {
  int width = 0, height = 0;
  glfwGetFramebufferSize(renderer->vkd->window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(renderer->vkd->window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(renderer->vkd->device);

  render_graph_start_swap_chain_recreation(renderer->renderGraph);
  swap_chain_deinit(renderer->vks);

  swap_chain_init(renderer->vks, renderer->vkd);
  render_graph_finish_swap_chain_recreation(renderer->renderGraph);
}

void renderer_update(renderer *renderer) {
  render_graph_update(renderer->renderGraph);
  render_state_update(renderer->renderState, renderer);
}

void renderer_update_unified_uniform_buffer_callback(void *data, size_t currentFrameInFlight,
                                                     global_uniform_buffer_data *globalData,
                                                     instances_uniform_buffer_data *instancesData) {

  renderer *renderer = data;

  // globals
  global_uniform_buffer_element *global =
      global_uniform_buffer_data_get_element(globalData, 0, currentFrameInFlight);
  render_pass_state_set_unified_uniform_buffer(renderer->renderGraph->renderPassState, global);

  // instances
  // HIRO move to batches
  utarray_foreach_elem_deref (
      renderer_cache_primitive_element *, primitiveElement,
      renderer->renderGraph->renderPassState->sharedState.rendererCacheBatches->primitiveElements) {

    assert(renderer_cache_primitive_is_valid(primitiveElement, instanceId));
    size_t instanceId = primitiveElement->instanceId;
    instances_uniform_buffer_element *element =
        instances_uniform_buffer_data_get_element(instancesData, instanceId, currentFrameInFlight);

    glm_mat4_copy(primitiveElement->transform, element->modelMat);

    assert(renderer_cache_primitive_is_valid(primitiveElement, materialElement));
    size_t materialId = primitiveElement->materialElement->materialIdx;
    element->materialId = materialId;
  }
}

void renderer_send_to_device(renderer *renderer) {
  render_graph_send_to_device(renderer->renderGraph);
  render_state_send_to_device(renderer->renderState);
}

void renderer_draw_frame(renderer *renderer) {
  // NOTE: Synchronization:
  //       https://www.intel.com/content/www/us/en/developer/articles/training/practical-approach-to-vulkan-part-1.html

  /* proceed to next frame */
  sync_advance_to_next_frame(renderer->renderState->sync);
  sync_wait_for_current_frame_fence(renderer->renderState->sync);

  /* acquire swap chain image */
  uint32_t swapChainImageIdx;
  VkResult result = vkAcquireNextImageKHR(
      renderer->vkd->device, renderer->vks->swapChain, UINT64_MAX,
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight],
      VK_NULL_HANDLE, &swapChainImageIdx);
  log_debug("acquired swap chain image #%u", swapChainImageIdx);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      renderer->vkd->framebufferResized) {
    renderer->vkd->framebufferResized = false;
    renderer_recreate_swap_chain(renderer);
    return;
  }
  verify(result == VK_SUCCESS);

  /* Pre-submit CPU work:
   * We have acquired index of next in-flight image, we can now update
   * frame-specific resources (uniform buffers, push constants). */
  renderer_update(renderer);
  renderer_send_to_device(renderer);

  /* record command buffer */
  VkCommandBuffer commandBuffer =
      sync_get_current_frame_command_buffer(renderer->renderState->sync);
  vkResetCommandBuffer(commandBuffer, 0);

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  verify(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);

  // Bind all render state.
  unified_geometry_buffer_record_bind_command(renderer->renderState->unifiedGeometryBuffer,
                                              commandBuffer);
  descriptors_record_bind_commands(
      renderer->renderState->descriptors, commandBuffer,
      (draw_push_constant_element){.currentFrameInFlight =
                                       renderer->renderState->sync->currentFrameInFlight});

  // Record render passes.
  render_graph_record_commands(renderer->renderGraph, commandBuffer, swapChainImageIdx);

  verify(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);

  /* submit command buffer */
  sync_reset_current_frame_fence(renderer->renderState->sync);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {
      renderer->renderState->sync
          ->imageAvailableSemaphores[renderer->renderState->sync->currentFrameInFlight]};

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
    renderer_recreate_swap_chain(renderer);
  } else {
    verify(result == VK_SUCCESS);
  }
}

void renderer_run_main_loop(renderer *renderer, renderer_main_loop_update_func updateFunc) {
  glfwMakeContextCurrent(renderer->vkd->window);

  double currentTime = glfwGetTime();
  while (glfwWindowShouldClose(renderer->vkd->window) == 0) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    double fps = 1.0 / frameTime;
    currentTime = newTime;
    while (frameTime > 0.0) {
      double dt = MIN(frameTime, MIN_DELTA_TIME);
      updateFunc(renderer, fps, dt);
      input_clear_released_keys(&renderer->vkd->input);
      frameTime -= dt;
    }
    glfwPollEvents(); // calls GLFW callbacks
    renderer_draw_frame(renderer);
  }
  vkDeviceWaitIdle(renderer->vkd->device);
}

void renderer_exit_main_loop(renderer *renderer) {
  glfwSetWindowShouldClose(renderer->vkd->window, true);
}

void renderer_debug_print(renderer *renderer) {
  log_debug("renderer:\n");
  scene_data_debug_print(renderer->data, 2);
  renderer_cache_debug_print(renderer->rendererCache);
  scene_graph_debug_print(renderer->sceneGraph);
  render_state_debug_print(renderer->renderState);
  render_graph_debug_print(renderer->renderGraph);
}

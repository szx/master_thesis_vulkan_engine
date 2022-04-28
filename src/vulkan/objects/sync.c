#include "sync.h"

vulkan_sync *vulkan_sync_create(vulkan_device *vkd) {
  vulkan_sync *sync = core_alloc(sizeof(vulkan_sync));

  sync->vkd = vkd;
  sync->currentFrameInFlight = 0;

  vulkan_queue_families queueFamilies = find_queue_families(sync->vkd, sync->vkd->physicalDevice);

  for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
    sync->imageAvailableSemaphores[i] =
        vulkan_create_semaphore(sync->vkd, 0, "frame #%zu available for rendering", i);
    sync->renderFinishedSemaphores[i] =
        vulkan_create_semaphore(sync->vkd, 0, "frame #%zu finished rendering", i);
    sync->inFlightFences[i] = vulkan_create_fence(sync->vkd, VK_FENCE_CREATE_SIGNALED_BIT,
                                                  "frame #%zu finished rendering", i);

    sync->commandPools[i] = vulkan_create_command_pool(
        sync->vkd, queueFamilies.graphicsFamily,
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // command buffer is short-lived,
                                                             // rerecorded every frame
        "frame #%zu", i);
    sync->commandBuffers[i] =
        vulkan_create_command_buffer(sync->vkd, sync->commandPools[i], "frame #%zu", i);
  }

  return sync;
}

void vulkan_sync_destroy(vulkan_sync *sync) {
  for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(sync->vkd->device, sync->imageAvailableSemaphores[i], vka);
    vkDestroySemaphore(sync->vkd->device, sync->renderFinishedSemaphores[i], vka);
    vkDestroyFence(sync->vkd->device, sync->inFlightFences[i], vka);

    vkFreeCommandBuffers(sync->vkd->device, sync->commandPools[i], 1, &sync->commandBuffers[i]);
    vkDestroyCommandPool(sync->vkd->device, sync->commandPools[i], vka);
  }

  core_free(sync);
}

void vulkan_sync_wait_for_current_frame_fence(vulkan_sync *sync) {
  vkWaitForFences(sync->vkd->device, 1, &sync->inFlightFences[sync->currentFrameInFlight], VK_TRUE,
                  UINT64_MAX);
}

void vulkan_sync_reset_current_frame_fence(vulkan_sync *sync) {
  vkResetFences(sync->vkd->device, 1, &sync->inFlightFences[sync->currentFrameInFlight]);
}

void vulkan_sync_advance_to_next_frame(vulkan_sync *sync) {
  sync->currentFrameInFlight = (sync->currentFrameInFlight + 1) % FRAMES_IN_FLIGHT;
}

VkCommandBuffer vulkan_sync_get_current_frame_command_buffer(vulkan_sync *sync) {
  return sync->commandBuffers[sync->currentFrameInFlight];
}

void vulkan_sync_debug_print(vulkan_sync *sync, int indent) {
  log_debug(INDENT_FORMAT_STRING "sync:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "currentFrameInFlight=%zu", INDENT_FORMAT_ARGS(2),
            sync->currentFrameInFlight);
}

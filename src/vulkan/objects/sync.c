#include "sync.h"

sync *sync_create(device *vkd) {
  sync *sync = core_alloc(sizeof(struct sync));

  sync->vkd = vkd;
  sync->currentFrameInFlight = 0;

  queue_families queueFamilies = find_queue_families(sync->vkd, sync->vkd->physicalDevice);

  for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
    sync->imageAvailableSemaphores[i] =
        create_semaphore(sync->vkd, 0, "frame #%zu available for rendering", i);
    sync->renderFinishedSemaphores[i] =
        create_semaphore(sync->vkd, 0, "frame #%zu finished rendering", i);
    sync->inFlightFences[i] =
        create_fence(sync->vkd, VK_FENCE_CREATE_SIGNALED_BIT, "frame #%zu finished rendering", i);

    sync->commandPools[i] = create_command_pool(
        sync->vkd, queueFamilies.graphicsFamily,
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // command buffer is short-lived,
                                                             // rerecorded every frame
        "frame #%zu", i);
    sync->commandBuffers[i] =
        create_command_buffer(sync->vkd, sync->commandPools[i], "frame #%zu", i);
  }

  return sync;
}

void sync_destroy(sync *sync) {
  for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(sync->vkd->device, sync->imageAvailableSemaphores[i], vka);
    vkDestroySemaphore(sync->vkd->device, sync->renderFinishedSemaphores[i], vka);
    vkDestroyFence(sync->vkd->device, sync->inFlightFences[i], vka);

    vkFreeCommandBuffers(sync->vkd->device, sync->commandPools[i], 1, &sync->commandBuffers[i]);
    vkDestroyCommandPool(sync->vkd->device, sync->commandPools[i], vka);
  }

  core_free(sync);
}

void sync_wait_for_current_frame_fence(sync *sync) {
  vkWaitForFences(sync->vkd->device, 1, &sync->inFlightFences[sync->currentFrameInFlight], VK_TRUE,
                  UINT64_MAX);
}

void sync_reset_current_frame_fence(sync *sync) {
  vkResetFences(sync->vkd->device, 1, &sync->inFlightFences[sync->currentFrameInFlight]);
}

void sync_advance_to_next_frame(sync *sync) {
  size_t nextFrameInFlight = (sync->currentFrameInFlight + 1) % FRAMES_IN_FLIGHT;
  log_debug("advancing from frame %zu to frame %zu", sync->currentFrameInFlight, nextFrameInFlight);
  sync->currentFrameInFlight = nextFrameInFlight;
}

VkCommandBuffer sync_get_current_frame_command_buffer(sync *sync) {
  return sync->commandBuffers[sync->currentFrameInFlight];
}

void sync_debug_print(sync *sync, int indent) {
  log_debug(INDENT_FORMAT_STRING "sync:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "currentFrameInFlight=%zu", INDENT_FORMAT_ARGS(2),
            sync->currentFrameInFlight);
}

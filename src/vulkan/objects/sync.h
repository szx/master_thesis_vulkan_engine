/* Vulkan API helper objects. */
#pragma once

#include "common.h"

/// Vulkan object used to synchronize rendering frames in flight.
typedef struct vulkan_sync {
  vulkan_device *vkd; ///< Pointer.

  /// Current frame rendered in flight (0..MAX_FRAMES_IN_FLIGHT).
  size_t currentFrameInFlight;

  /// Semaphore signaling that frame has been acquired from swap chain and is ready for rendering.
  VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];

  /// Semaphore signaling that rendering frame is finished and it can be presented.
  VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];

  /// Fence that is signaled after submitted command buffer have completed rendering frame.
  VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];

} vulkan_sync;

vulkan_sync *vulkan_sync_create(vulkan_device *vkd);
void vulkan_sync_destroy(vulkan_sync *sync);

void vulkan_sync_wait_for_current_frame_fence(vulkan_sync *sync);

void vulkan_sync_reset_current_frame_fence(vulkan_sync *sync);

void vulkan_sync_advance_to_next_frame(vulkan_sync *sync);

void vulkan_sync_debug_print(vulkan_sync *sync, int indent);

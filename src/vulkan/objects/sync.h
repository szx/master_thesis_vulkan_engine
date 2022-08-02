/* Vulkan API helper objects. */
#pragma once

#include "../common.h"

/// Vulkan objects used by pipelines to render frames in flight.
typedef struct sync {
  device *vkd; ///< Pointer.

  /// Current frame rendered in flight (0..FRAMES_IN_FLIGHT).
  size_t currentFrameInFlight;

  /// Semaphore signaling that frame has been acquired from swap chain and is ready for rendering.
  VkSemaphore imageAvailableSemaphores[FRAMES_IN_FLIGHT];

  /// Semaphore signaling that rendering frame is finished and it can be presented.
  VkSemaphore renderFinishedSemaphores[FRAMES_IN_FLIGHT];

  /// Fence that is signaled after submitted command buffer have completed rendering frame.
  VkFence inFlightFences[FRAMES_IN_FLIGHT];

  /// Command pool used to record command buffers.
  VkCommandPool commandPools[FRAMES_IN_FLIGHT];

  /// Command buffers.
  VkCommandBuffer commandBuffers[FRAMES_IN_FLIGHT];
} sync;

sync *sync_create(device *vkd);
void sync_destroy(sync *sync);

void sync_wait_for_current_frame_fence(sync *sync);

void sync_reset_current_frame_fence(sync *sync);

void sync_advance_to_next_frame(sync *sync);

VkCommandBuffer sync_get_current_frame_command_buffer(sync *sync);

void sync_debug_print(sync *sync, int indent);

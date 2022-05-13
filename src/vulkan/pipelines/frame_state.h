/* Pipeline frame state.
 * All objects unique to pipeline.
 * Pipeline maintains number of framebuffer states equal to number of swap chain images.
 * Pipeline maintains number of frame states equal to FRAMES_IN_FLIGHT.
 */
#pragma once

#include "../objects/image.h"

typedef struct vulkan_pipeline vulkan_pipeline;

/// Manages framebuffer used by pipeline's render pass.
typedef struct vulkan_pipeline_framebuffer_state {
  vulkan_pipeline *pipeline; ///< Pointer.

  /// Framebuffer used to write into swap chain (onscreen rendering) into images (offscreen
  /// rendering).
  VkFramebuffer framebuffer;

} vulkan_pipeline_framebuffer_state;

void vulkan_pipeline_framebuffer_state_init(vulkan_pipeline_framebuffer_state *framebufferState,
                                            vulkan_pipeline *pipeline, uint32_t swapChainImageIdx);
void vulkan_pipeline_framebuffer_state_deinit(vulkan_pipeline_framebuffer_state *framebufferState);

/// Manages frame-specific resources (indirect buffer) used by pipeline to render on
/// frame.
typedef struct vulkan_pipeline_frame_state {
  vulkan_pipeline *pipeline; ///< Pointer.
  void *impl;                ///< Pointer to struct with frame state used by pipeline impl.
} vulkan_pipeline_frame_state;

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline);
void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_update(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent);

/* Vulkan API helper objects. */

#pragma once

#include "batch.h"
#include "image.h"
#include "render_cache_list.h"
#include "render_state.h"

typedef struct vulkan_pipeline vulkan_pipeline;

/// Manages frame-specific resources (command buffer, framebuffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_frame_state {
  vulkan_pipeline *pipeline; ///< Pointer.

  /// Framebuffer used to write into swap chain (onscreen rendering) or into images
  /// (offscreen rendering).
  VkFramebuffer framebuffer;

  // HIRO HIRO differs between pipelines, just use void* ?
  vulkan_batches_data batchesData;
} vulkan_pipeline_frame_state;

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline, uint32_t swapChainImageIdx);
void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent);

/// Manages resources shared between frames (depth buffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_shared_state {
  vulkan_swap_chain *vks; ///< Pointer.

  /// Depth buffer image.
  /// We can share it between pipelines, because it is synchronized using pipeline barriers.
  vulkan_image *depthBufferImage;
} vulkan_pipeline_shared_state;

vulkan_pipeline_shared_state *vulkan_pipeline_shared_state_create(vulkan_swap_chain *vks);
void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *pipelineSharedState);

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *pipelineSharedState,
                                       vulkan_swap_chain *vks);
void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *pipelineSharedState);

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *pipelineSharedState,
                                              int indent);

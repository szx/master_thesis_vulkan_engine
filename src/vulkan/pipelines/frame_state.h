/* Pipeline frame state.
 * All objects unique to pipeline.
 * Pipeline maintains number of framebuffer states equal to number of swap chain images.
 * Pipeline maintains number of frame states equal to FRAMES_IN_FLIGHT.
 */
#pragma once

#include "../objects/image.h"
#include "../renderers/batch.h"

typedef struct vulkan_pipeline_state vulkan_pipeline_state;

/// Manages frame-specific resources (indirect buffer) used by pipelines.
typedef struct vulkan_pipeline_frame_state {

  vulkan_pipeline_state *pipelineState; /// Pointer.

  /// Frame in flight index.
  size_t frameInFlight;

  // HIRO Maintain two batches for transparent and opaque objects.
  vulkan_batches_buffer rendererCacheBatchesData;
} vulkan_pipeline_frame_state;

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline_state *pipelineState, size_t frameInFlight);
void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_update(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_send_to_device(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent);

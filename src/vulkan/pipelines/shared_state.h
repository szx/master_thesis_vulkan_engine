/* Pipeline shared state.
 * All objects that can be shared between pipelines.
 */
#pragma once

#include "../renderers/batch.h"
#include "../renderers/render_state.h"
#include "camera_state.h"
#include "light_state.h"
#include "skybox_state.h"

/// Manages resources shared between frames (depth buffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_shared_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_pipeline_camera_state *camera;
  vulkan_pipeline_light_state *lights;
  vulkan_pipeline_skybox_state *skybox;

  // HIRO: Maintain two batches for transparent and opaque objects.
  vulkan_batches *renderCacheListBatches;

  /// Depth buffer image.
  /// We can share it between pipelines, because it is synchronized using pipeline barriers.
  vulkan_image *depthBufferImage;

} vulkan_pipeline_shared_state;

vulkan_pipeline_shared_state *vulkan_pipeline_shared_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *pipelineSharedState);

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *pipelineSharedState,
                                       vulkan_render_state *renderState);
void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *pipelineSharedState);
void vulkan_pipeline_shared_state_reinit_with_new_swap_chain(
    vulkan_pipeline_shared_state *pipelineSharedState);

void vulkan_pipeline_shared_state_update(vulkan_pipeline_shared_state *pipelineSharedState,
                                         vulkan_batch_instancing_policy instancingPolicy);

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *pipelineSharedState,
                                              int indent);

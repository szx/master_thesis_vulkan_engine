/* Pipeline shared state.
 * All objects that can be shared between pipelines.
 */
#pragma once

#include "../renderers/batch.h"
#include "../renderers/render_state.h"
#include "camera_state.h"
#include "font_state.h"
#include "g_buffer_state.h"
#include "light_state.h"
#include "material_state.h"
#include "skybox_state.h"

/// Manages resources shared between frames (depth buffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_shared_state {
  vulkan_render_state *renderState; ///< Pointer.

  /** CPU state used to update uniform buffers.
   * Can (should) be changed only in main loop's updateFunc, which prevents synchronization
   * issues. */
  vulkan_pipeline_camera_state *camera;
  vulkan_pipeline_material_state *materials;
  vulkan_pipeline_light_state *lights;
  vulkan_pipeline_skybox_state *skybox;
  vulkan_pipeline_font_state *font;
  vulkan_pipeline_g_buffer_state *gBuffer;

  // HIRO: Maintain two batches for transparent and opaque objects
  // HIRO: in batches recording multiple commands controlled by sorting with multiple policies?
  vulkan_batches *rendererCacheBatches;

  /// Depth buffer image.
  /// We can share it between pipelines, because it is synchronized using pipeline barriers.
  vulkan_image *depthBufferImage;

} vulkan_pipeline_shared_state;

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *sharedState,
                                       vulkan_render_state *renderState);
void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *sharedState);
void vulkan_pipeline_shared_state_reinit_with_new_swap_chain(
    vulkan_pipeline_shared_state *sharedState);

void vulkan_pipeline_shared_state_update(vulkan_pipeline_shared_state *sharedState);
void vulkan_pipeline_shared_state_set_unified_uniform_buffer(
    vulkan_pipeline_shared_state *sharedState, vulkan_global_uniform_buffer_element *global);
void vulkan_pipeline_shared_state_send_to_device(vulkan_pipeline_shared_state *sharedState);

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *sharedState,
                                              int indent);

/* Pipeline shared state.
 * All objects that can be shared between renderPasss.
 */
#pragma once

#include "../../batch.h"
#include "../../render_state.h"
#include "camera_state.h"
#include "font_state.h"
#include "light_state.h"
#include "material_state.h"
#include "skybox_state.h"

/// Manages resources shared between frames (depth buffer) used by renderPass to render one
/// one swap chain frame.
typedef struct render_pass_shared_state {
  render_state *renderState; ///< Pointer.

  /** Pipeline state used to update uniform buffers and provide read-only textures.
   * CPU state can (should) be changed only in main loop's updateFunc, which prevents
   * synchronization issues. GPU state (textures) are read-only, which prevents synchronization
   * issues. */
  render_pass_camera_state *camera;
  render_pass_material_state *materials;
  render_pass_light_state *lights;
  render_pass_skybox_state *skybox;
  render_pass_font_state *font;

  // HIRO: Maintain two batches for transparent and opaque objects
  // HIRO: in batches recording multiple commands controlled by sorting with multiple policies?
  batches *rendererCacheBatches;

} render_pass_shared_state;

void render_pass_shared_state_init(render_pass_shared_state *sharedState,
                                   render_state *renderState);
void render_pass_shared_state_deinit(render_pass_shared_state *sharedState);
void render_pass_shared_state_reinit_with_new_swap_chain(render_pass_shared_state *sharedState);

void render_pass_shared_state_update(render_pass_shared_state *sharedState);
void render_pass_shared_state_set_unified_constant_buffer(render_pass_shared_state *sharedState,
                                                          global_uniform_buffer_element *global);
void render_pass_shared_state_send_to_device(render_pass_shared_state *sharedState);

void render_pass_shared_state_debug_print(render_pass_shared_state *sharedState, int indent);

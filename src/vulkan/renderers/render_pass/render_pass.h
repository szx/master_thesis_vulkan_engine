/* Render passes.
 */

#pragma once

#include "render_pass_state.h"
#include "shader_program/shader_program.h"

/// Used to record command to command buffer.
/// Can be chained with other render passes using render graph.
typedef struct render_pass {
  render_state *renderState;          ///< Pointer.
  render_pass_state *renderPassState; ///< Pointer.

  render_pass_desc desc;
  render_pass_shader_program *shaderProgram;

  /* cache */
  UT_array *_graphicsPipelines; // VkPipeline array

} render_pass;

render_pass *render_pass_create(render_pass_desc desc, render_state *renderState,
                                render_pass_state *renderPassState);
void render_pass_destroy(render_pass *renderPass);

void render_pass_init(render_pass *renderPass, render_pass_desc desc, render_state *renderState,
                      render_pass_state *renderPassState);
void render_pass_deinit(render_pass *renderPass);

void render_pass_record_commands(render_pass *renderPass, VkCommandBuffer commandBuffer,
                                 size_t swapChainImageIdx, rendering_info renderPassInfo);

void render_pass_debug_print(render_pass *renderPass);

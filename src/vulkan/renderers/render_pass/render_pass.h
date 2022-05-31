/* Render passes.
 */

#pragma once

#include "render_pass_impl.h"
#include "render_pass_state.h"
#include "shader_program/shader_program.h"

/// Used to record command to command buffer.
/// Can be chained with other render passes using render graph.
typedef struct vulkan_render_pass {
  vulkan_render_state *renderState;          ///< Pointer.
  vulkan_render_pass_state *renderPassState; ///< Pointer.

  vulkan_render_pass_type type;
  vulkan_render_pass_info info;
  vulkan_render_pass_shader_program *shaderProgram;

  /* cache */
  UT_array *_renderPasses;      // VkRenderPass array
  UT_array *_framebuffers;      // VkFramebuffer array
  UT_array *_graphicsPipelines; // VkPipeline array

} vulkan_render_pass;

vulkan_render_pass *vulkan_render_pass_create(vulkan_render_pass_type type,
                                              vulkan_render_state *renderState,
                                              vulkan_render_pass_state *renderPassState);
void vulkan_render_pass_destroy(vulkan_render_pass *renderPass);

void vulkan_render_pass_init(vulkan_render_pass *renderPass, vulkan_render_pass_type type,
                             vulkan_render_state *renderState,
                             vulkan_render_pass_state *renderPassState);
void vulkan_render_pass_deinit(vulkan_render_pass *renderPass);

vulkan_render_pass_info vulkan_render_pass_get_info(vulkan_render_pass *renderPass);

void vulkan_render_pass_record_commands(vulkan_render_pass *renderPass,
                                        VkCommandBuffer commandBuffer, size_t swapChainImageIdx,
                                        vulkan_render_pass_create_info renderPassCreateInfo);

void vulkan_render_pass_debug_print(vulkan_render_pass *renderPass);

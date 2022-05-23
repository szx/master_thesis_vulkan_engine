/* Generic pipeline.
 */

#pragma once

#include "frame_state.h"
#include "pipeline_impl.h"
#include "pipeline_state.h"
#include "shader_program.h"

/// Used to record command to command buffer.
/// Can be chained with other pipelines.
typedef struct vulkan_pipeline {
  vulkan_swap_chain *vks;               ///< Pointer.
  vulkan_render_state *renderState;     ///< Pointer.
  vulkan_pipeline_state *pipelineState; ///< Pointer.

  vulkan_pipeline_type type;
  vulkan_pipeline_shader_program *shaderProgram;

  /// Pipeline used by renderer before this pipeline (or NULL if first).
  struct vulkan_pipeline *prev;
  /// Pipeline used by renderer before this pipeline (or NULL if last).
  struct vulkan_pipeline *next;

  /* cache */
  UT_array *_renderPasses;      // VkRenderPass array
  UT_array *_framebuffers;      // VkFramebuffer array
  UT_array *_graphicsPipelines; // VkPipeline array

} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create_start(vulkan_pipeline_type type, vulkan_swap_chain *vks,
                                              vulkan_render_state *renderState,
                                              vulkan_pipeline_state *pipelineState);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

void vulkan_pipeline_init_start(vulkan_pipeline *pipeline, vulkan_pipeline_type type,
                                vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                vulkan_pipeline_state *pipelineState);
void vulkan_pipeline_init_prev_next(vulkan_pipeline *pipeline, vulkan_pipeline *prev,
                                    vulkan_pipeline *next);
void vulkan_pipeline_init_finish(vulkan_pipeline *pipeline);
void vulkan_pipeline_deinit(vulkan_pipeline *pipeline);

vulkan_pipeline_info vulkan_pipeline_get_pipeline_info(vulkan_pipeline *pipeline);

void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                        size_t swapChainImageIdx);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);

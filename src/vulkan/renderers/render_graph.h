/* Render graph.
 */

// NOTE: Information about render graphs:
// https://ourmachinery.com/post/high-level-rendering-using-render-graphs/
// https://docs.unity3d.com/Packages/com.unity.render-pipelines.core@10.2/manual/render-graph-system.html
// https://andrewcjp.wordpress.com/2019/09/28/the-render-graph-architecture/
// https://apoorvaj.io/render-graphs-1/

#pragma once

#include "render_pass/render_pass.h"

typedef struct vulkan_render_graph vulkan_render_graph;
typedef struct vulkan_render_graph_render_pass_element vulkan_render_graph_render_pass_element;

#define MAX_RENDER_PASS_COUNT 16

/* render graph resource element */

typedef struct vulkan_image_render_pass_usage_timeline {
  vulkan_image_render_pass_usage usages[MAX_RENDER_PASS_COUNT];
  VkFormat formats[MAX_RENDER_PASS_COUNT];
  VkClearValue clearValues[MAX_RENDER_PASS_COUNT];
} vulkan_image_render_pass_usage_timeline;

void vulkan_image_render_pass_usage_timeline_add_new_usage(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    vulkan_image_render_pass_usage usage);

void vulkan_image_render_pass_usage_timeline_add_new_format(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx, VkFormat format);

void vulkan_image_render_pass_usage_timeline_add_new_clear_value(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkClearValue clearValue);

void vulkan_image_render_pass_usage_timeline_debug_print(
    vulkan_image_render_pass_usage_timeline *usageTimeline, int indent);

typedef struct vulkan_image_render_pass_usage_timeline_info {
  vulkan_image_render_pass_usage previousUsage;
  vulkan_image_render_pass_usage currentUsage;
  vulkan_image_render_pass_usage nextUsage;
  VkFormat previousFormat;
  VkFormat currentFormat;
  VkFormat nextFormat;
  VkClearValue clearValue;
} vulkan_image_render_pass_usage_timeline_info;

vulkan_image_render_pass_usage_timeline_info vulkan_image_render_pass_usage_timeline_get_info(
    vulkan_image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx);

/// Edge of render graph.
typedef struct vulkan_render_graph_resource {
  vulkan_image_type imageType;

  /// Info collected before compilation.
  vulkan_image_render_pass_usage_timeline usageTimeline;

  struct vulkan_render_graph_resource *prev, *next;

} vulkan_render_graph_resource;

void vulkan_render_graph_resource_init(vulkan_render_graph_resource *element,
                                       vulkan_image_type imageType);

void vulkan_render_graph_resource_deinit(vulkan_render_graph_resource *element);

void vulkan_render_graph_resource_debug_print(vulkan_render_graph_resource *element);

/* render graph render pass element */

/// Render pass.
/// Node of render graph.
typedef struct vulkan_render_graph_render_pass_element {
  /// Index between 0 and MAX_RENDER_PASS_COUNT.
  size_t idx;

  /// Render pass.
  vulkan_render_pass *renderPass;

  /// vulkan_render_graph_resource* array of offscreen textures that can be:
  ///     - read from by render pass' fragment shader
  ///     - written to by render pass via framebuffer color attachments
  UT_array *offscreenTextures;

  /// Swap chain image used as render target.
  vulkan_render_graph_resource *swapChainImageResource;

  /// Depth buffer used for:
  /// - depth test
  /// - depth write
  vulkan_render_graph_resource *depthBufferResource;

  /// State used during compilation.
  struct vulkan_render_graph_render_pass_element_state {
    vulkan_render_pass_info renderPassInfo;
  } state;

  struct vulkan_render_graph_render_pass_element *prev, *next;

} vulkan_render_graph_render_pass_element;

vulkan_render_graph_render_pass_element *
vulkan_render_graph_render_pass_element_create(size_t renderPassIdx, vulkan_render_pass *renderPass,
                                               vulkan_render_graph *renderGraph);

void vulkan_render_graph_render_pass_element_destroy(
    vulkan_render_graph_render_pass_element *element);

void vulkan_render_graph_render_pass_element_debug_print(
    vulkan_render_graph_render_pass_element *element);

/* render graph */

/// Render graph,
typedef struct vulkan_render_graph {

  vulkan_render_state *renderState; ///< Pointer.

  /// Global state shared by all render passes.
  vulkan_render_pass_state *renderPassState;

  /// List of render graph render pass elements.
  /// Ordered by execution order.
  vulkan_render_graph_render_pass_element *renderPassElements;

  /// List of unique render graph offscreen texture resource elements.
  vulkan_render_graph_resource *resources;

  size_t _renderPassIdx;
  bool _compiled;

} vulkan_render_graph;

vulkan_render_graph *vulkan_render_graph_create(vulkan_render_state *renderState);

void vulkan_render_graph_destroy(vulkan_render_graph *renderGraph);

void vulkan_render_graph_start_swap_chain_recreation(vulkan_render_graph *renderGraph);

void vulkan_render_graph_finish_swap_chain_recreation(vulkan_render_graph *renderGraph);

/// Adds render pass and it's resources.
void vulkan_render_graph_add_render_pass(vulkan_render_graph *renderGraph,
                                         vulkan_render_pass_desc desc);

void vulkan_render_graph_compile(vulkan_render_graph *renderGraph);

void vulkan_render_graph_record_commands(vulkan_render_graph *renderGraph,
                                         VkCommandBuffer commandBuffer, size_t swapChainImageIdx);

void vulkan_render_graph_update(vulkan_render_graph *renderGraph);

void vulkan_render_graph_send_to_device(vulkan_render_graph *renderGraph);

void vulkan_render_graph_debug_print(vulkan_render_graph *renderGraph);

vulkan_render_graph_resource *
vulkan_render_graph_add_image_resource(vulkan_render_graph *renderGraph,
                                       vulkan_image_type imageType);

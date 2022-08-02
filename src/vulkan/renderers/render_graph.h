/* Render graph.
 */

// NOTE: Information about render graphs:
// https://ourmachinery.com/post/high-level-rendering-using-render-graphs/
// https://docs.unity3d.com/Packages/com.unity.render-pipelines.core@10.2/manual/render-graph-system.html
// https://andrewcjp.wordpress.com/2019/09/28/the-render-graph-architecture/
// https://apoorvaj.io/render-graphs-1/

#pragma once

#include "render_pass/render_pass.h"

typedef struct render_graph render_graph;
typedef struct render_graph_render_pass_element render_graph_render_pass_element;

#define MAX_RENDER_PASS_COUNT 16

/* render graph resource element */

typedef struct image_render_pass_usage_timeline {
  image_render_pass_usage usages[MAX_RENDER_PASS_COUNT];
  VkFormat formats[MAX_RENDER_PASS_COUNT];
  VkClearValue clearValues[MAX_RENDER_PASS_COUNT];
} image_render_pass_usage_timeline;

void image_render_pass_usage_timeline_add_new_usage(image_render_pass_usage_timeline *usageTimeline,
                                                    size_t renderGraphIdx,
                                                    image_render_pass_usage usage);

void image_render_pass_usage_timeline_add_new_format(
    image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx, VkFormat format);

void image_render_pass_usage_timeline_add_new_clear_value(
    image_render_pass_usage_timeline *usageTimeline, size_t renderGraphIdx,
    VkClearValue clearValue);

void image_render_pass_usage_timeline_debug_print(image_render_pass_usage_timeline *usageTimeline,
                                                  int indent);

typedef struct image_render_pass_usage_timeline_info {
  image_render_pass_usage previousUsage;
  image_render_pass_usage currentUsage;
  image_render_pass_usage nextUsage;
  VkFormat previousFormat;
  VkFormat currentFormat;
  VkFormat nextFormat;
  VkClearValue clearValue;
} image_render_pass_usage_timeline_info;

image_render_pass_usage_timeline_info
image_render_pass_usage_timeline_get_info(image_render_pass_usage_timeline *usageTimeline,
                                          size_t renderGraphIdx);

/// Edge of render graph.
typedef struct render_graph_resource {
  const char *name;
  image_type imageType;
  uint32_t offscreenTextureIdx;

  /// Info collected before compilation.
  image_render_pass_usage_timeline usageTimeline;

  struct render_graph_resource *prev, *next;

} render_graph_resource;

void render_graph_resource_init(render_graph_resource *element, const char *name,
                                image_type imageType);

void render_graph_resource_deinit(render_graph_resource *element);

void render_graph_resource_debug_print(render_graph_resource *element);

/* render graph render pass element */

/// Render pass.
/// Node of render graph.
typedef struct render_graph_render_pass_element {
  /// Index between 0 and MAX_RENDER_PASS_COUNT.
  size_t idx;

  /// Render pass.
  render_pass *renderPass;

  /// render_graph_resource* array of offscreen textures that can be:
  ///     - read from by render pass' fragment shader
  ///     - written to by render pass via framebuffer color attachments
  UT_array *offscreenTextures;

  /// Swap chain image used as render target.
  render_graph_resource *swapChainImageResource;

  /// Depth buffer used for:
  /// - depth test
  /// - depth write
  render_graph_resource *depthBufferResource;

  /// State used during compilation.
  struct render_graph_render_pass_element_state {
    rendering_info renderPassInfo;
  } state;

  struct render_graph_render_pass_element *prev, *next;

} render_graph_render_pass_element;

render_graph_render_pass_element *
render_graph_render_pass_element_create(size_t renderPassIdx, render_pass *renderPass,
                                        render_graph *renderGraph);

void render_graph_render_pass_element_destroy(render_graph_render_pass_element *element);

void render_graph_render_pass_element_debug_print(render_graph_render_pass_element *element);

/* render graph */

/// Render graph,
typedef struct render_graph {

  render_state *renderState; ///< Pointer.

  /// Global state shared by all render passes.
  render_pass_state *renderPassState;

  /// List of render graph render pass elements.
  /// Ordered by execution order.
  render_graph_render_pass_element *renderPassElements;

  /// List of unique render graph offscreen texture resource elements.
  render_graph_resource *resources;

  size_t _renderPassIdx;
  bool _compiledResources;
  bool _compiledRenderPasses;

} render_graph;

render_graph *render_graph_create(render_state *renderState);

void render_graph_destroy(render_graph *renderGraph);

void render_graph_start_swap_chain_recreation(render_graph *renderGraph);

void render_graph_finish_swap_chain_recreation(render_graph *renderGraph);

/// Adds render pass and it's resources.
void render_graph_add_render_pass(render_graph *renderGraph, render_pass_desc desc);

void render_graph_compile(render_graph *renderGraph);

void render_graph_record_commands(render_graph *renderGraph, VkCommandBuffer commandBuffer,
                                  size_t swapChainImageIdx);

void render_graph_update(render_graph *renderGraph);

void render_graph_send_to_device(render_graph *renderGraph);

void render_graph_debug_print(render_graph *renderGraph);

render_graph_resource *render_graph_add_image_resource(render_graph *renderGraph, const char *name,
                                                       image_type imageType);
render_graph_resource *render_graph_get_image_resource(render_graph *renderGraph, const char *name);

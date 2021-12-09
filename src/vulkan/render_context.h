/* Rendering Vulkan scene. */

#ifndef VULKAN_RENDER_CONTEXT_H
#define VULKAN_RENDER_CONTEXT_H

#include "../platform.h"
#include "swap_chain.h"

/// Manages frame-specific resources (command buffer, framebuffer) used to
/// render one swap chain frame.
typedef struct vulkan_swap_chain_frame {
  vulkan_device *vkd;     /// vulkan_device pointer.
  vulkan_swap_chain *vks; /// vulkan_swap_chain pointer.
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
  VkFramebuffer framebuffer;    /// Vulkan framebuffer. Used in conjunction with
                                /// render pass. Used by swap chain.
  uint32_t swapChainImageIndex; /// Index in vks->swapChainImages of swap chain
                                /// image associated with framebuffer.
} vulkan_swap_chain_frame;

vulkan_swap_chain_frame
vulkan_swap_chain_frame_init(vulkan_swap_chain *vks,
                             uint32_t swapChainImageIndex);
void vulkan_swap_chain_frame_free(vulkan_swap_chain_frame *frame);
vulkan_swap_chain_frame
vulkan_swap_chain_frame_copy(vulkan_swap_chain_frame *frame);

#define T vulkan_swap_chain_frame
#include "vec.h" // vec_vulkan_swap_chain_frame

typedef enum vulkan_render_pass_type {
  ForwardRenderPass
} vulkan_render_pass_type;

/// Describes one render pass in pipeline.
typedef struct vulkan_render_pass {
  vulkan_device *vkd;     /// vulkan_device pointer.
  vulkan_swap_chain *vks; /// vulkan_swap_chain pointer.
  vulkan_render_pass_type type;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
} vulkan_render_pass;

vulkan_render_pass vulkan_render_pass_init(vulkan_swap_chain *vks,
                                           vulkan_render_pass_type type);
void vulkan_render_pass_free(vulkan_render_pass *renderPass);

/// Manages frame-agnostic resources and frames used to render into swap chain.
typedef struct vulkan_render_context {
  vulkan_device *vkd;                          /// Vulkan device (heap).
  vulkan_swap_chain *vks;                      /// Vulkan swap chain (heap).
  vec_vulkan_swap_chain_frame swapChainFrames; /// Swap chain frames.
  // TODO: Synchronization.
  // TODO: Rendering pipeline.
  vulkan_render_pass *renderPass; /// Render pass (heap).
} vulkan_render_context;

vulkan_render_context vulkan_render_context_init(data_config *config);
void vulkan_render_context_free(vulkan_render_context *renderContext);

#include "../codegen/render_context.h"

#endif /* !VULKAN_RENDER_CONTEXT_H */

/* Rendering Vulkan scene. */

#ifndef VULKAN_RENDER_CONTEXT_H
#define VULKAN_RENDER_CONTEXT_H

#include "../core/platform.h"
#include "scene.h"
#include "shader.h"
#include "swap_chain.h"

typedef enum vulkan_render_pass_type { ForwardRenderPass } vulkan_render_pass_type;

/// Describes one render pass in pipeline.
typedef struct vulkan_render_pass {
  vulkan_device *vkd;     /// vulkan_device pointer.
  vulkan_swap_chain *vks; /// vulkan_swap_chain pointer.
  vulkan_render_pass_type type;
  // TODO: More render pass types, configuration.
  VkRenderPass renderPass;
  vulkan_shader *vertShader;
  vulkan_shader *fragShader;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
} vulkan_render_pass;

void vulkan_render_pass_init(vulkan_render_pass *renderPass, vulkan_swap_chain *vks,
                             vulkan_render_pass_type type);
void vulkan_render_pass_deinit(vulkan_render_pass *renderPass);

/// Describes rendering pipeline.
typedef struct vulkan_pipeline {
  vulkan_device *vkd;     /// vulkan_device pointer.
  vulkan_swap_chain *vks; /// vulkan_swap_chain pointer.
  // VkDescriptorPool descriptorPool;
  vulkan_render_pass *renderPass;
  // TODO: More render passes, configuration.
  // TODO: Descriptors, attachments. (store in vulkan_swap_chain_frame?)
} vulkan_pipeline;

void vulkan_pipeline_init(vulkan_pipeline *pipeline, vulkan_swap_chain *vks);
void vulkan_pipeline_deinit(vulkan_pipeline *pipeline);

/// Manages frame-specific resources (command buffer, framebuffer) used to
/// render one swap chain frame.
typedef struct vulkan_swap_chain_frame {
  vulkan_pipeline *pipeline; /// vulkan_pipeline pointer.
  vulkan_device *vkd;        /// vulkan_device pointer.
  vulkan_swap_chain *vks;    /// vulkan_swap_chain pointer.
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
  VkFramebuffer framebuffer;    /// Vulkan framebuffer. Used in conjunction with
                                /// render pass. Used by swap chain.
  uint32_t swapChainImageIndex; /// Index in vks->swapChainImages of swap chain
                                /// image associated with framebuffer.
} vulkan_swap_chain_frame;

void vulkan_swap_chain_frame_init(vulkan_swap_chain_frame *frame, vulkan_pipeline *pipeline,
                                  uint32_t swapChainImageIndex);
void vulkan_swap_chain_frame_deinit(vulkan_swap_chain_frame *frame);
vulkan_swap_chain_frame vulkan_swap_chain_frame_copy(vulkan_swap_chain_frame *frame);

/// Manages frame-agnostic resources and frames used to render into swap chain.
typedef struct vulkan_render_context {
  vulkan_device *vkd;                       /// Vulkan device.
  vulkan_swap_chain *vks;                   /// Vulkan swap chain.
  vulkan_pipeline *pipeline;                /// Rendering pipeline.
  vulkan_swap_chain_frame *swapChainFrames; /// Swap chain frames.
  vulkan_scene *scene;                      /// Vulkan scene.
  size_t currentFrameInFlight;              /// Current frame rendered in flight.
  /// Semaphore signaling that frame has been acquired from swap chain and is ready for drawing.
  VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  /// Semaphore signaling that drawing frame is finished and it can be presented.
  VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  /// Fence that is signaled after submitted command buffer have completed drawing frame.
  VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
  /// Handle to fence that will be signaled after submitted command buffer finishes drawing frame.
  VkFence imagesInFlight[MAX_FRAMES_IN_FLIGHT];
} vulkan_render_context;

void vulkan_render_context_init(vulkan_render_context *rctx, data_config *config);
void vulkan_render_context_deinit(vulkan_render_context *rctx);
/// Recreate vulkan_render_context when swap chain is out-of-date.
void vulkan_render_context_recreate_swap_chain(vulkan_render_context *rctx);
void vulkan_render_context_load_scene(vulkan_render_context *rctx, char *sceneName);
void vulkan_render_context_send_scene_to_gpu(vulkan_render_context *rctx);

void vulkan_render_context_draw_frame(vulkan_render_context *rctx);
void vulkan_pipeline_record_frame_command_buffer(vulkan_pipeline *pipeline,
                                                 vulkan_swap_chain_frame *frame);

#include "../codegen/render_context.h"

#endif /* !VULKAN_RENDER_CONTEXT_H */

/* Rendering Vulkan scene. */
#pragma once

#include "../core/platform.h"
#include "scene.h"
#include "shader.h"
#include "swap_chain.h"

// TODO: More render pass types, configuration.
typedef enum vulkan_render_pass_type { ForwardRenderPass } vulkan_render_pass_type;

/// Describes created render pass.
/// Used by vulkan_render_pass_record_frame_command_buffer() to determine if scene can be drawn
/// using current render pass, or if render pass needs tuning.
typedef struct vulkan_render_pass_info {
  bool usesPushConstants;
  bool usesSceneUniformBuffer;
  vulkan_attribute_type supportedVertexAttributes;
} vulkan_render_pass_info;

typedef struct vulkan_pipeline vulkan_pipeline;

/// Describes one render pass in pipeline.
typedef struct vulkan_render_pass {
  vulkan_device *vkd;        /// vulkan_device pointer.
  vulkan_swap_chain *vks;    /// vulkan_swap_chain pointer.
  vulkan_pipeline *pipeline; /// parent vulkan_pipeline pointer.
  vulkan_render_pass_type type;
  vulkan_render_pass_info info;
  VkRenderPass renderPass;
  vulkan_shader *vertShader;
  vulkan_shader *fragShader;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
} vulkan_render_pass;

vulkan_render_pass *vulkan_render_pass_create(vulkan_pipeline *pipeline,
                                              vulkan_render_pass_type type);
void vulkan_render_pass_destroy(vulkan_render_pass *renderPass);
/// Validate render pass.
void vulkan_render_pass_validate(vulkan_render_pass *renderPass, vulkan_scene *scene);

/// Describes rendering pipeline.
typedef struct vulkan_pipeline {
  vulkan_device *vkd;     /// vulkan_device pointer.
  vulkan_swap_chain *vks; /// vulkan_swap_chain pointer.
  vulkan_scene *scene;    /// parent scene pointer

  // TODO create descriptors using scene
  VkDescriptorPool descriptorPool;
  VkDescriptorSetLayout descriptorSetLayout;
  VkDescriptorSet descriptorSet;

  vulkan_render_pass *renderPass;
  // TODO: More render passes, configuration.
  // TODO: Descriptors, attachments. (store in vulkan_swap_chain_frame?)
} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_scene *scene);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

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
  vulkan_device *vkd;                                  /// Vulkan device.
  vulkan_swap_chain *vks;                              /// Vulkan swap chain.
  vulkan_pipeline *pipeline;                           /// Rendering pipeline.
  core_array(vulkan_swap_chain_frame) swapChainFrames; /// Swap chain frames.
  vulkan_scene *scene;                                 /// Vulkan scene.
  size_t currentFrameInFlight;                         /// Current frame rendered in flight.
  /// Semaphore signaling that frame has been acquired from swap chain and is ready for rendering.
  VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  /// Semaphore signaling that rendering frame is finished and it can be presented.
  VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  /// Fence that is signaled after submitted command buffer have completed rendering frame.
  VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
  /// Handle to fence that will be signaled after submitted command buffer finishes rendering frame.
  VkFence imagesInFlight[MAX_FRAMES_IN_FLIGHT];
} vulkan_render_context;

void vulkan_render_context_init(vulkan_render_context *rctx, data_config *config,
                                data_assets *assets, char *sceneName);
void vulkan_render_context_deinit(vulkan_render_context *rctx);
/// Recreate vulkan_render_context when swap chain is out-of-date.
void vulkan_render_context_recreate_swap_chain(vulkan_render_context *rctx);
/// Load scene.
void vulkan_render_context_load_scene(vulkan_render_context *rctx, char *sceneName);

/// Update render_context data on CPU. Can make scene dirty.
void vulkan_render_context_update_data(vulkan_render_context *rctx);
/// Send dirty scene resources (geometry buffer, uniform buffer) to GPU.
void vulkan_render_context_send_to_device(vulkan_render_context *rctx);

/// Acquires swap chain frame.
/// Records frame command buffer using vulkan_pipeline_record_frame_command_buffer().
/// Submits frame command buffer.
void vulkan_render_context_draw_frame(vulkan_render_context *rctx);

/// Records frame command buffer for scene using rendering pipeline.
/// Calls vulkan_render_pass_record_frame_command_buffer() for each render pass in rendering
/// pipeline.
void vulkan_pipeline_record_frame_command_buffer(vulkan_scene *scene, vulkan_pipeline *pipeline,
                                                 vulkan_swap_chain_frame *frame);

/// Records frame command buffer for scene using single render pass of rendering pipeline.
void vulkan_render_pass_record_frame_command_buffer(vulkan_scene *scene,
                                                    vulkan_render_pass *renderPass,
                                                    vulkan_swap_chain_frame *frame);


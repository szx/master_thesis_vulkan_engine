/* Vulkan API helper functions. */

#pragma once

#include "../../core/core.h"

typedef struct device device;

uint32_t device_find_memory_type(device *vkd, uint32_t typeFilter,
                                 VkMemoryPropertyFlags properties);
VkFormat device_find_supported_format(device *vkd, VkImageTiling tiling,
                                      VkFormatFeatureFlags features, VkFormat *candidates,
                                      size_t candidateCount);
size_t device_format_size(VkFormat format);
VkIndexType device_stride_to_index_type(size_t stride);

/* vulkan object creation */

VkCommandPool device_create_command_pool(device *vkd, uint32_t queueFamilyIndex,
                                         VkCommandPoolCreateFlags flags, const char *debugFormat,
                                         ...);

VkCommandBuffer device_create_command_buffer(device *vkd, VkCommandPool commandPool,
                                             const char *debugFormat, ...);

VkImage device_create_image(device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                            uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                            VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                            const char *debugFormat, ...);

VkDeviceMemory device_create_image_memory(device *vkd, VkImage image,
                                          VkMemoryPropertyFlags properties, const char *debugFormat,
                                          ...);

VkImageView device_create_image_view(device *vkd, VkImage image, VkImageViewType type,
                                     VkFormat format, VkImageAspectFlags aspectFlags,
                                     uint32_t mipLevels, uint32_t arrayLayers,
                                     const char *debugFormat, ...);

VkSampler device_create_sampler(device *vkd, uint32_t mipLevelCount, const char *debugFormat, ...);

void device_create_buffer(device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer *buffer,
                          VkDeviceMemory *bufferMemory, const char *debugFormat, ...);

VkShaderModule device_create_shader_module(device *vkd, const uint32_t *code, size_t size,
                                           const char *debugFormat, ...);

VkDescriptorPool device_create_descriptor_pool(device *vkd, size_t totalUniformBufferCount,
                                               size_t totalCombinedImageSamplerCount,
                                               size_t maxAllocatedDescriptorSetsCount,
                                               const char *debugFormat, ...);

VkPipelineLayout device_create_pipeline_layout(device *vkd, VkPipelineLayoutCreateFlags flags,
                                               const VkDescriptorSetLayout *descriptorSetLayouts,
                                               size_t descriptorSetLayoutCount,
                                               const VkPushConstantRange *pushConstantRanges,
                                               size_t pushConstantRangeCount,
                                               const char *debugFormat, ...);

typedef struct rendering_attachment_info {
  const char *name;
  VkImageView imageView;

  VkFormat currentFormat;
  VkImageLayout currentLayout;
  VkAttachmentLoadOp loadOp;
  VkAttachmentStoreOp storeOp;
  VkClearValue clearValue;
} rendering_attachment_info;

typedef struct rendering_image_layout_transition_info {
  const char *name;
  VkImage image;
  VkImageAspectFlags imageAspectFlags;

  VkImageLayout oldLayout;
  VkImageLayout newLayout;
} rendering_image_layout_transition_info;

typedef struct rendering_info {
  rendering_attachment_info *onscreenColorAttachment;
  rendering_attachment_info *depthAttachment;
  UT_array *offscreenColorAttachments;

  UT_array *preImageLayoutTransition;
  UT_array *postImageLayoutTransition;
} rendering_info;

void rendering_info_init(rendering_info *renderPassInfo);

void rendering_info_deinit(rendering_info *renderPassInfo);

void rendering_info_add_onscreen_color_attachment(rendering_info *renderPassInfo,
                                                  rendering_attachment_info attachmentCreateInfo);

void rendering_info_add_offscreen_color_attachment(rendering_info *renderPassInfo,
                                                   rendering_attachment_info attachmentCreateInfo);

void rendering_info_add_depth_attachment(rendering_info *renderPassInfo,
                                         rendering_attachment_info attachmentCreateInfo);

void rendering_info_pre_image_layout_transition(
    rendering_info *renderPassInfo,
    rendering_image_layout_transition_info imageLayoutTransitionInfo);

void rendering_info_post_image_layout_transition(
    rendering_info *renderPassInfo,
    rendering_image_layout_transition_info imageLayoutTransitionInfo);

void device_begin_rendering(device *vkd, VkCommandBuffer commandBuffer,
                            rendering_info renderPassInfo);

void device_end_rendering(device *vkd, VkCommandBuffer commandBuffer,
                          rendering_info renderPassInfo);

typedef enum color_blending_type {
  color_blending_type_none,
  color_blending_type_alpha,
  color_blending_type_count,
} color_blending_type;

typedef struct graphics_pipeline_create_info {
  uint32_t colorAttachmentCount;
  color_blending_type colorBlendingType;

  bool depthWriteEnable;
  bool depthTestEnable;
  VkCompareOp depthTestOp;

  VkPipelineShaderStageCreateInfo *shaderStages;
  uint32_t shaderStageCount;

  const VkVertexInputBindingDescription *vertexInputBindingDescriptions;
  size_t vertexBindingDescriptionsCount;
  const VkVertexInputAttributeDescription *vertexAttributeDescriptions;
  size_t vertexAttributeDescriptionsCount;

  uint32_t framebufferWidth;
  uint32_t framebufferHeight;

  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
} graphics_pipeline_create_info;

VkPipeline device_create_graphics_pipeline(device *vkd, graphics_pipeline_create_info createInfo,
                                           rendering_info renderingInfo, const char *debugFormat,
                                           ...);

VkSemaphore device_create_semaphore(device *vkd, VkSemaphoreCreateFlags flags,
                                    const char *debugFormat, ...);

VkFence device_create_fence(device *vkd, VkFenceCreateFlags flags, const char *debugFormat, ...);

/* one-shot commands */

VkCommandBuffer device_begin_one_shot_commands(device *vkd);
void device_end_one_shot_commands(device *vkd);

void device_create_staging_buffer(device *vkd, VkDeviceSize size, VkBuffer *buffer,
                                  VkDeviceMemory *bufferMemory);

void device_one_shot_copy_buffer_to_buffer(device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                           VkDeviceSize size);
void device_one_shot_copy_buffer_to_image(device *vkd, VkBuffer buffer, VkImage image,
                                          uint32_t width, uint32_t height, uint32_t layerCount);

void device_one_shot_generate_mipmaps(device *vkd, VkImage image, VkFormat format, uint32_t width,
                                      uint32_t height, uint32_t mipLevelCount);

void device_one_shot_transition_image_layout(device *vkd, VkImage image,
                                             VkImageAspectFlags imageAspectFlags,
                                             VkImageLayout oldLayout, VkImageLayout newLayout);

/* general commands */

void device_transition_image_layout_command(device *vkd, VkCommandBuffer commandBuffer,
                                            VkImage image, VkImageAspectFlags imageAspectFlags,
                                            uint32_t baseMipLevel, uint32_t mipLevelCount,
                                            VkImageLayout oldLayout, VkImageLayout newLayout);

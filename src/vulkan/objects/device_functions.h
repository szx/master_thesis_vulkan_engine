/* Vulkan API helper functions. */

#pragma once

#include "../../core/core.h"

typedef struct vulkan_device vulkan_device;

uint32_t vulkan_find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                                 VkMemoryPropertyFlags properties);
VkFormat vulkan_find_supported_format(vulkan_device *vkd, VkImageTiling tiling,
                                      VkFormatFeatureFlags features, VkFormat *candidates,
                                      size_t candidateCount);
VkFormat vulkan_find_depth_format(vulkan_device *vkd);
size_t vulkan_format_size(VkFormat format);
VkIndexType vulkan_stride_to_index_type(size_t stride);

/* vulkan object creation */

VkCommandPool vulkan_create_command_pool(vulkan_device *vkd, uint32_t queueFamilyIndex,
                                         VkCommandPoolCreateFlags flags, const char *debugFormat,
                                         ...);

VkCommandBuffer vulkan_create_command_buffer(vulkan_device *vkd, VkCommandPool commandPool,
                                             const char *debugFormat, ...);

VkFramebuffer vulkan_create_framebuffer(vulkan_device *vkd, VkRenderPass renderPass,
                                        uint32_t attachmentCount, const VkImageView *attachments,
                                        uint32_t width, uint32_t height, const char *debugFormat,
                                        ...);

VkImage vulkan_create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                            uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                            VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                            const char *debugFormat, ...);

VkDeviceMemory vulkan_create_image_memory(vulkan_device *vkd, VkImage image,
                                          VkMemoryPropertyFlags properties, const char *debugFormat,
                                          ...);

VkImageView vulkan_create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                                     VkFormat format, VkImageAspectFlags aspectFlags,
                                     uint32_t mipLevels, uint32_t arrayLayers,
                                     const char *debugFormat, ...);

VkSampler vulkan_create_sampler(vulkan_device *vkd, uint32_t mipLevelCount, const char *debugFormat,
                                ...);

void vulkan_create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer *buffer,
                          VkDeviceMemory *bufferMemory, const char *debugFormat, ...);

VkShaderModule vulkan_create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size,
                                           const char *debugFormat, ...);

VkDescriptorPool vulkan_create_descriptor_pool(vulkan_device *vkd, size_t totalUniformBufferCount,
                                               size_t totalCombinedImageSamplerCount,
                                               size_t maxAllocatedDescriptorSetsCount,
                                               bool bindless, const char *debugFormat, ...);

VkPipelineLayout vulkan_create_pipeline_layout(vulkan_device *vkd,
                                               VkPipelineLayoutCreateFlags flags,
                                               const VkDescriptorSetLayout *descriptorSetLayouts,
                                               size_t descriptorSetLayoutCount,
                                               const VkPushConstantRange *pushConstantRanges,
                                               size_t pushConstantRangeCount,
                                               const char *debugFormat, ...);

typedef enum vulkan_color_blending_type {
  vulkan_color_blending_type_none,
  vulkan_color_blending_type_alpha,
  vulkan_color_blending_type_count,
} vulkan_color_blending_type;

VkPipeline vulkan_create_graphics_pipeline(
    vulkan_device *vkd,

    vulkan_color_blending_type colorBlendingType,

    VkPipelineShaderStageCreateInfo *shaderStages, uint32_t shaderStageCount,

    const VkVertexInputBindingDescription *vertexInputBindingDescriptions,
    size_t vertexBindingDescriptionsCount,
    const VkVertexInputAttributeDescription *vertexAttributeDescriptions,
    size_t vertexAttributeDescriptionsCount,

    uint32_t framebufferWidth, uint32_t framebufferHeight,

    const VkDescriptorSetLayout *descriptorSetLayouts, size_t descriptorSetLayoutCount,
    const VkPushConstantRange *pushConstantRanges, size_t pushConstantRangeCount,

    VkRenderPass renderPass, VkPipelineLayout pipelineLayout, const char *debugFormat, ...);

VkRenderPass vulkan_create_render_pass(
    vulkan_device *vkd, VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    size_t offscreenAttachmentDescriptionCount,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    size_t offscreenAttachmentReferenceCount, VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference, const char *debugFormat, ...);

VkSemaphore vulkan_create_semaphore(vulkan_device *vkd, VkSemaphoreCreateFlags flags,
                                    const char *debugFormat, ...);

VkFence vulkan_create_fence(vulkan_device *vkd, VkFenceCreateFlags flags, const char *debugFormat,
                            ...);

/* one-shot commands */

VkCommandBuffer vulkan_begin_one_shot_commands(vulkan_device *vkd);
void vulkan_end_one_shot_commands(vulkan_device *vkd, VkCommandBuffer commandBuffer);

void vulkan_copy_buffer_to_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                  VkDeviceSize size);
void vulkan_copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image, uint32_t width,
                                 uint32_t height, uint32_t baseArrayLayer);

void vulkan_generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat format, uint32_t width,
                             uint32_t height, uint32_t mipLevelCount);

void vulkan_transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                                    VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t mipLevels, uint32_t arrayLayers);

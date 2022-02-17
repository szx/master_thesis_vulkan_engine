/* Vulkan API helper functions. */

#pragma once

#include "../core/platform.h"
typedef struct vulkan_device vulkan_device;

// TODO: Implement all Vulkan helper functions.

uint32_t find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
VkFormat find_depth_format(vulkan_device *vkd);

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

void vulkan_create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                         uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                         VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage *image,
                         VkDeviceMemory *imageMemory, const char *debugFormat, ...);

VkImageView vulkan_create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                                     VkFormat format, VkImageAspectFlags aspectFlags,
                                     uint32_t mipLevels, uint32_t arrayLayers,
                                     const char *debugFormat, ...);

void vulkan_create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer *buffer,
                          VkDeviceMemory *bufferMemory, const char *debugFormat, ...);

VkShaderModule vulkan_create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size,
                                           const char *debugFormat, ...);

VkDescriptorPool vulkan_create_descriptor_pool(vulkan_device *vkd, size_t totalUniformBufferCount,
                                               size_t totalCombinedImageSamplerCount,
                                               size_t maxAllocatedDescriptorSetsCount,
                                               const char *debugFormat, ...);

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(vulkan_device *vkd,
                                                          VkDescriptorType descriptorType,
                                                          uint32_t descriptorCount,
                                                          VkShaderStageFlags stageFlags,
                                                          const char *debugFormat, ...);

VkDescriptorSet vulkan_create_descriptor_set_for_uniform_buffers(
    vulkan_device *vkd, VkBuffer *uniformBuffers, VkDeviceSize bufferSize, size_t bufferCount,
    VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
    const char *debugFormat, ...);

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

void vulkan_generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat,
                             int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
void vulkan_transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                                    VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t mipLevels, uint32_t arrayLayers);

// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::set<std::shared_ptr<Texture>> textures, VkDescriptorSetLayout
// descriptorSetLayout, VkDescriptorPool descriptorPool);
// std::tuple<VkPipelineLayout, VkPipeline> createGraphicsPipeline(PipelineType
// type, RenderPassName name, const ShaderConstants &shaderConstants,
// VkRenderPass renderPass, size_t numOutputColorAttachments, uint32_t width,
// uint32_t height, std::vector<VkDescriptorSetLayout> setLayouts);

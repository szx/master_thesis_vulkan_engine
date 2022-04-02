/* Vulkan API helper functions. */

#pragma once

#include "../core/core.h"
typedef struct vulkan_device vulkan_device;
typedef struct vulkan_shader_program vulkan_shader_program;
typedef struct vulkan_render_state vulkan_render_state;
typedef struct vulkan_swap_chain vulkan_swap_chain;
typedef struct vulkan_descriptor_binding vulkan_descriptor_binding;
typedef struct vulkan_data_texture vulkan_data_texture;

// TODO: Implement all Vulkan helper functions.

uint32_t vulkan_find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                                 VkMemoryPropertyFlags properties);
VkFormat vulkan_find_supported_format(vulkan_device *vkd, VkImageTiling tiling,
                                      VkFormatFeatureFlags features, VkFormat *candidates,
                                      size_t candidateCount);
VkFormat vulkan_find_depth_format(vulkan_device *vkd);
VkFormat vulkan_find_texture_format(vulkan_device *vkd, vulkan_data_texture *texture);
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
                                               const char *debugFormat, ...);

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(vulkan_device *vkd,
                                                          vulkan_descriptor_binding *bindings,
                                                          size_t bindingCount,
                                                          const char *debugFormat, ...);

VkDescriptorSet vulkan_create_descriptor_set(vulkan_device *vkd,
                                             VkDescriptorSetLayout descriptorSetLayout,
                                             VkDescriptorPool descriptorPool,
                                             vulkan_descriptor_binding *bindings,
                                             size_t bindingCount, const char *debugFormat, ...);

VkPipelineLayout vulkan_create_pipeline_layout(vulkan_device *vkd,
                                               VkPipelineLayoutCreateFlags flags,
                                               const VkDescriptorSetLayout *descriptorSetLayouts,
                                               size_t descriptorSetLayoutCount,
                                               const VkPushConstantRange *pushConstantRanges,
                                               size_t pushConstantRangeCount,
                                               const char *debugFormat, ...);

VkPipeline vulkan_create_graphics_pipeline(
    vulkan_device *vkd, vulkan_shader_program *shaderProgram, vulkan_render_state *renderState,
    vulkan_swap_chain *vks,

    const VkDescriptorSetLayout *descriptorSetLayouts, size_t descriptorSetLayoutCount,
    const VkPushConstantRange *pushConstantRanges, size_t pushConstantRangeCount,

    VkRenderPass renderPass, VkPipelineLayout *pipelineLayout, const char *debugFormat, ...);

VkRenderPass vulkan_create_render_pass(
    vulkan_device *vkd, VkAttachmentDescription *colorAttachmentDescriptions,
    size_t colorAttachmentDescriptionCount, VkAttachmentReference *colorAttachmentReferences,
    size_t colorAttachmentReferenceCount, VkAttachmentDescription depthAttachmentDescription,
    VkAttachmentReference depthAttachmentReference, const char *debugFormat, ...);

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

// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::set<std::shared_ptr<Texture>> textures, VkDescriptorSetLayout
// descriptorSetLayout, VkDescriptorPool descriptorPool);

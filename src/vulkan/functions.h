/* Vulkan API and helper functions and structures */

#pragma once

#include "../core/platform.h"
typedef struct vulkan_device vulkan_device;

/* Vulkan helper structures */

/// One element of vertex stream.
/// Assumes that all vertex attributes are interleaved.
/// If vertex attribute is not used, we have to do one of following:
///     - adjust vertex binding stride and shader (if it is after all other used vertex attributes)
///     - fill vertex attribute with default value
typedef struct vulkan_vertex_stream_element {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
} vulkan_vertex_stream_element;
// TODO: Packing.

typedef enum vulkan_attribute_type {
  UnknownAttribute = 1 << 0, // TODO naming
  PositionAttribute = 1 << 1,
  NormalAttribute = 1 << 2,
  ColorAttribute = 1 << 3,
  TexCoordAttribute = 1 << 4,
  TangentAttribute = 1 << 5,
} vulkan_attribute_type;

typedef enum vulkan_index_type {
  vulkan_index_type_unknown = 0,
  vulkan_index_type_uint32 = 1
} vulkan_index_type;

typedef struct vulkan_geometry_buffer {
  /* initialized by vulkan_scene_build_geometry_buffer */
  UT_array *data; /// uint8_t
  /* initialized by vulkan_geometry_buffer_send_to_device */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;
  /* state */
  bool dirty; /// True if geometry buffer data updated on CPU.
} vulkan_geometry_buffer;

typedef struct vulkan_uniform_buffer {
  /* initialized by vulkan_uniform_buffer_create */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;
  VkDeviceSize bufferMemorySize;
  /* initialized and updated by vulkan_uniform_buffer_send_to_device */
  struct {
    alignas(16) mat4 viewMat;
    alignas(16) mat4 projMat;
  } data;
  /* state */
  bool dirty;            /// True if uniform buffer data updated on CPU.
} vulkan_uniform_buffer; // TODO: Manage every uniform buffers.

/* Vulkan helper functions */
// TODO: Implement all Vulkan helper functions.

vulkan_geometry_buffer *vulkan_geometry_buffer_create();
void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer);
/// Creates device local buffer and copies geometry buffer data into it.
void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer);

/// Creates host visible and coherent buffer.
/// Descriptor set is created and managed by pipeline.
/// NOTE: We do not use device local memory, because we assume that mapping host visible and
///       coherent memory is faster than copying into device local memory using staging buffer.
vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd);
void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer);
/// Maps and updates uniform buffer memory.
void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer);

uint32_t find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
VkFormat find_depth_format(vulkan_device *vkd);

uint32_t vertex_types_to_vertex_stride(vulkan_attribute_type vertexAttributes);
uint32_t index_type_to_index_stride(vulkan_index_type indexType);
vulkan_index_type index_stride_to_index_type(uint32_t indexStride);
VkIndexType stride_to_index_format(uint32_t indexStride);

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

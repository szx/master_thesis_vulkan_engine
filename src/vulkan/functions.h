/* Vulkan API and helper functions and structures */

#ifndef VULKAN_FUNCTIONS_H
#define VULKAN_FUNCTIONS_H

#include "device.h"

/* Vulkan helper structures */

/// One element of vertex stream.
/// We assume that all vertex attributes are interleaved.
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
  UnknownAttribute = 1 << 0, // HIRO naming
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
  UT_array *data; /// uint8_t, initialized by vulkan_scene_build_geometry_buffer
  /*  initialized by vulkan_geometry_buffer_send_to_device */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;
} vulkan_geometry_buffer;

/* Vulkan helper functions */
// TODO: Implement all Vulkan helper functions.

vulkan_geometry_buffer *vulkan_geometry_buffer_create();
void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer);

/// Creates device local buffer and copies geometry buffer data into it.
void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer);

uint32_t find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
VkFormat find_depth_format(vulkan_device *vkd);

uint32_t vertex_types_to_vertex_stride(vulkan_attribute_type vertexTypes);
uint32_t index_type_to_index_stride(vulkan_index_type indexType);
vulkan_index_type index_stride_to_index_type(uint32_t indexStride);
VkIndexType stride_to_index_format(uint32_t indexStride);

void create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                  uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                  VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage image, VkDeviceMemory imageMemory);
VkImageView create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                              VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              uint32_t arrayLayers);
void create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer *buffer,
                   VkDeviceMemory *bufferMemory);

VkCommandBuffer begin_single_time_commands(vulkan_device *vkd);
void end_single_time_commands(vulkan_device *vkd, VkCommandBuffer commandBuffer);

void copy_buffer_to_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                           VkDeviceSize size);
void copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image, uint32_t width,
                          uint32_t height, uint32_t baseArrayLayer);

void generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat, int32_t texWidth,
                      int32_t texHeight, uint32_t mipLevels);
void transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                             uint32_t arrayLayers);

VkShaderModule create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size);
// VkDescriptorSetLayout create_descriptor_set_layout(VkDescriptorType
// descriptorType, uint32_t descriptorCount, VkShaderStageFlags stageFlags);
// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::vector<VkBuffer> uniformBuffers, VkDeviceSize bufferSize,
// VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::set<std::shared_ptr<Texture>> textures, VkDescriptorSetLayout
// descriptorSetLayout, VkDescriptorPool descriptorPool);
// std::tuple<VkPipelineLayout, VkPipeline> createGraphicsPipeline(PipelineType
// type, RenderPassName name, const ShaderConstants &shaderConstants,
// VkRenderPass renderPass, size_t numOutputColorAttachments, uint32_t width,
// uint32_t height, std::vector<VkDescriptorSetLayout> setLayouts);

#include "../codegen/functions.h"   // HIRO remove
#include "../codegen/vulkan_core.h" // HIRO remove

#endif /* !VULKAN_FUNCTIONS_H */

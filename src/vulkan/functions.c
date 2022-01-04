#include "functions.h"
#include "../codegen/functions.c"

vulkan_geometry_buffer *vulkan_geometry_buffer_create() {
  vulkan_geometry_buffer *geometryBuffer = core_alloc(sizeof(vulkan_geometry_buffer));
  static const UT_icd ut_vulkan_geometry_buffer_data_icd = {sizeof(uint8_t), NULL, NULL, NULL};
  utarray_new(geometryBuffer->data, &ut_vulkan_geometry_buffer_data_icd);
  geometryBuffer->vkd = NULL;
  geometryBuffer->buffer = VK_NULL_HANDLE;
  geometryBuffer->bufferMemory = VK_NULL_HANDLE;
  return geometryBuffer;
}

void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer) {
  utarray_free(geometryBuffer->data);
  if (geometryBuffer->vkd != NULL) {
    vkDestroyBuffer(geometryBuffer->vkd->device, geometryBuffer->buffer, vka);
    vkFreeMemory(geometryBuffer->vkd->device, geometryBuffer->bufferMemory, vka);
    geometryBuffer->vkd = NULL;
    geometryBuffer->buffer = VK_NULL_HANDLE;
    geometryBuffer->bufferMemory = VK_NULL_HANDLE;
  }
  core_free(geometryBuffer);
}

void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer) {
  // HIRO: Reuse staging buffer.
  // HIRO: Check if geometry buffer is dirty.
  // HIRO: Free geometry buffer data if geometry buffer is device local.
  geometryBuffer->vkd = vkd;
  size_t geometryBufferSize = utarray_len(geometryBuffer->data);
  uint8_t *geometryBufferData = utarray_front(geometryBuffer->data);
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer(geometryBuffer->vkd, geometryBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &stagingBuffer, &stagingBufferMemory);

  void *data;
  vkMapMemory(geometryBuffer->vkd->device, stagingBufferMemory, 0, geometryBufferSize, 0, &data);
  memcpy(data, geometryBufferData, geometryBufferSize);
  vkUnmapMemory(geometryBuffer->vkd->device, stagingBufferMemory);

  create_buffer(geometryBuffer->vkd, geometryBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &geometryBuffer->buffer,
                &geometryBuffer->bufferMemory);

  copy_buffer_to_buffer(geometryBuffer->vkd, stagingBuffer, geometryBuffer->buffer,
                        geometryBufferSize);

  vkDestroyBuffer(geometryBuffer->vkd->device, stagingBuffer, vka);
  vkFreeMemory(geometryBuffer->vkd->device, stagingBufferMemory, vka);
}

uint32_t find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(vkd->physicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
  return 0;
}

VkFormat find_depth_format(vulkan_device *vkd) { return VK_FORMAT_R32_UINT; }

uint32_t vertex_types_to_vertex_stride(vulkan_attribute_type vertexTypes) {
  if ((vertexTypes & TexCoordAttribute) != 0) {
    return offsetof(vulkan_vertex_stream_element, texCoord) +
           member_size(vulkan_vertex_stream_element, texCoord);
  }
  if ((vertexTypes & ColorAttribute) != 0) {
    return offsetof(vulkan_vertex_stream_element, color) +
           member_size(vulkan_vertex_stream_element, color);
  }
  if ((vertexTypes & NormalAttribute) != 0) {
    return offsetof(vulkan_vertex_stream_element, normal) +
           member_size(vulkan_vertex_stream_element, normal);
  }
  if ((vertexTypes & PositionAttribute) != 0) {
    return offsetof(vulkan_vertex_stream_element, position) +
           member_size(vulkan_vertex_stream_element, position);
  }
  return 0; // TODO: Unreachable.
}

uint32_t index_type_to_index_stride(vulkan_index_type indexType) {
  switch (indexType) {
  case vulkan_index_type_uint16:
    return 2;
  case vulkan_index_type_uint32:
    return 4;
  }
  return 0; // HIRO unreachable
}

vulkan_index_type index_stride_to_index_type(uint32_t indexStride) {
  switch (indexStride) {
  case 2:
    return vulkan_index_type_uint16;
  case 4:
    return vulkan_index_type_uint32;
  }
  return 0; // HIRO unreachable
}

VkIndexType stride_to_index_format(uint32_t indexStride) {
  VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
  if (indexStride == 2) {
    indexType = VK_INDEX_TYPE_UINT16;
  } else if (indexStride == 4) {
    indexType = VK_INDEX_TYPE_UINT32;
  }
  return indexType;
}

void create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                  uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                  VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage image, VkDeviceMemory imageMemory) {}

VkImageView create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                              VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              uint32_t arrayLayers) {
  VkImageViewCreateInfo viewInfo = {0};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = type;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = arrayLayers;

  VkImageView imageView;
  verify(vkCreateImageView(vkd->device, &viewInfo, vka, &imageView) == VK_SUCCESS);

  return imageView;
}

void create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer *buffer,
                   VkDeviceMemory *bufferMemory) {
  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  verify(vkCreateBuffer(vkd->device, &bufferInfo, vka, buffer) == VK_SUCCESS);

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkd->device, *buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory_type(vkd, memRequirements.memoryTypeBits, properties);
  verify(vkAllocateMemory(vkd->device, &allocInfo, vka, bufferMemory) == VK_SUCCESS);
  vkBindBufferMemory(vkd->device, *buffer, *bufferMemory, 0);
}

VkCommandBuffer begin_single_time_commands(vulkan_device *vkd) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vkd->oneShotCommandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(vkd->device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void end_single_time_commands(vulkan_device *vkd, VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(vkd->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(vkd->graphicsQueue);

  vkFreeCommandBuffers(vkd->device, vkd->oneShotCommandPool, 1, &commandBuffer);
}

void copy_buffer_to_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                           VkDeviceSize size) {
  VkCommandBuffer commandBuffer = begin_single_time_commands(vkd);

  VkBufferCopy copyRegion = {0};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  end_single_time_commands(vkd, commandBuffer);
}

void copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image, uint32_t width,
                          uint32_t height, uint32_t baseArrayLayer) {}

void generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat, int32_t texWidth,
                      int32_t texHeight, uint32_t mipLevels) {}

void transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                             uint32_t arrayLayers) {}

VkShaderModule create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size) {
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size * sizeof(char);
  createInfo.pCode = code;

  VkShaderModule shaderModule;
  verify(vkCreateShaderModule(vkd->device, &createInfo, vka, &shaderModule) == VK_SUCCESS);
  return shaderModule;
}

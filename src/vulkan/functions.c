#include "functions.h"

vulkan_geometry_buffer *vulkan_geometry_buffer_create() {
  vulkan_geometry_buffer *geometryBuffer = core_alloc(sizeof(vulkan_geometry_buffer));
  static const UT_icd ut_vulkan_geometry_buffer_data_icd = {sizeof(uint8_t), NULL, NULL, NULL};
  utarray_new(geometryBuffer->data, &ut_vulkan_geometry_buffer_data_icd);
  geometryBuffer->vkd = NULL;
  geometryBuffer->buffer = VK_NULL_HANDLE;
  geometryBuffer->bufferMemory = VK_NULL_HANDLE;
  geometryBuffer->dirty = true;
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
  if (!geometryBuffer->dirty) {
    return;
  }
  assert(geometryBuffer->buffer == VK_NULL_HANDLE);
  assert(geometryBuffer->bufferMemory == VK_NULL_HANDLE);
  // TODO: Reuse staging buffer.
  // TODO: Free geometry buffer data if geometry buffer is device local.
  geometryBuffer->vkd = vkd;
  size_t geometryBufferSize = utarray_len(geometryBuffer->data);
  uint8_t *geometryBufferData = utarray_front(geometryBuffer->data);
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  create_buffer(geometryBuffer->vkd, geometryBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &stagingBuffer, &stagingBufferMemory, "staging buffer for geometry");

  void *data;
  vkMapMemory(geometryBuffer->vkd->device, stagingBufferMemory, 0, geometryBufferSize, 0, &data);
  memcpy(data, geometryBufferData, geometryBufferSize);
  vkUnmapMemory(geometryBuffer->vkd->device, stagingBufferMemory);

  create_buffer(geometryBuffer->vkd, geometryBufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &geometryBuffer->buffer,
                &geometryBuffer->bufferMemory, "geometry buffer");

  copy_buffer_to_buffer(geometryBuffer->vkd, stagingBuffer, geometryBuffer->buffer,
                        geometryBufferSize);

  vkDestroyBuffer(geometryBuffer->vkd->device, stagingBuffer, vka);
  vkFreeMemory(geometryBuffer->vkd->device, stagingBufferMemory, vka);
  geometryBuffer->dirty = false;
}

vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd) {
  vulkan_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_uniform_buffer));
  glm_mat4_identity(uniformBuffer->data.viewMat);
  glm_mat4_identity(uniformBuffer->data.projMat);
  uniformBuffer->vkd = vkd;
  uniformBuffer->bufferMemorySize = sizeof(uniformBuffer->data);
  create_buffer(uniformBuffer->vkd, uniformBuffer->bufferMemorySize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &uniformBuffer->buffer, &uniformBuffer->bufferMemory, "uniform buffer");
  uniformBuffer->dirty = true;
  return uniformBuffer;
  // TODO support multiple buffers and descriptors (seperate for each frame)
}

void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer) {
  assert(uniformBuffer->vkd != NULL);
  vkDestroyBuffer(uniformBuffer->vkd->device, uniformBuffer->buffer, vka);
  vkFreeMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory, vka);
  uniformBuffer->vkd = NULL;
  uniformBuffer->buffer = VK_NULL_HANDLE;
  uniformBuffer->bufferMemory = VK_NULL_HANDLE;
  uniformBuffer->bufferMemorySize = 0;
  core_free(uniformBuffer);
}

void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer) {
  assert(uniformBuffer->buffer != VK_NULL_HANDLE);
  assert(uniformBuffer->bufferMemory != VK_NULL_HANDLE);
  if (!uniformBuffer->dirty) {
    return;
  }
  void *data;
  vkMapMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory, 0,
              uniformBuffer->bufferMemorySize, 0, &data);
  memcpy(data, &uniformBuffer->data, uniformBuffer->bufferMemorySize);
  vkUnmapMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory);
  uniformBuffer->dirty = false;
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
  panic("unsupported vertex attribute %d", vertexTypes);
  return 0; // TODO: Unreachable.
}

uint32_t index_type_to_index_stride(vulkan_index_type indexType) {
  if (indexType == vulkan_index_type_uint32) {
    return 4;
  }
  panic("unsupported index type %d", indexType);
  return 0; // TODO unreachable
}

vulkan_index_type index_stride_to_index_type(uint32_t indexStride) {
  switch (indexStride) {
  case 4:
    return vulkan_index_type_uint32;
  default:
    return vulkan_index_type_unknown;
  }
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
                  VkMemoryPropertyFlags properties, VkImage image, VkDeviceMemory imageMemory) {
  assert(0);
}

VkImageView create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                              VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              uint32_t arrayLayers, const char *debugName) {
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
  vulkan_debug_name_image_view(vkd->debug, imageView, "%s - image view (%s)", debugName,
                               VkImageViewType_debug_str(viewInfo.viewType));

  return imageView;
}

void create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory,
                   const char *debugName) {
  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  verify(vkCreateBuffer(vkd->device, &bufferInfo, vka, buffer) == VK_SUCCESS);
  vulkan_debug_name_buffer(vkd->debug, *buffer, "%s - buffer (%zu B)", debugName, bufferInfo.size);

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkd->device, *buffer, &memRequirements);
  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory_type(vkd, memRequirements.memoryTypeBits, properties);
  verify(vkAllocateMemory(vkd->device, &allocInfo, vka, bufferMemory) == VK_SUCCESS);
  vulkan_debug_name_device_memory(vkd->debug, *bufferMemory, "%s - device memory (%zu B)",
                                  debugName, memRequirements.size);

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
  vulkan_debug_name_command_buffer(vkd->debug, commandBuffer, "one-shot command buffer");

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
                          uint32_t height, uint32_t baseArrayLayer) {
  assert(0);
}

void generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat, int32_t texWidth,
                      int32_t texHeight, uint32_t mipLevels) {
  assert(0);
}

void transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                             uint32_t arrayLayers) {
  assert(0);
}

VkShaderModule create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size,
                                    const char *debugName) {
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size * sizeof(char);
  createInfo.pCode = code;

  VkShaderModule shaderModule;
  verify(vkCreateShaderModule(vkd->device, &createInfo, vka, &shaderModule) == VK_SUCCESS);
  vulkan_debug_name_shader_module(vkd->debug, shaderModule, "%s - shader module", debugName);
  return shaderModule;
}

VkDescriptorPool create_descriptor_pool(vulkan_device *vkd, size_t totalUniformBufferCount,
                                        size_t totalCombinedImageSamplerCount,
                                        size_t maxAllocatedDescriptorSetsCount) {
  VkDescriptorPoolSize poolSizes[2] = {0};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = totalUniformBufferCount;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = totalCombinedImageSamplerCount;

  verify(poolSizes[0].descriptorCount <= vkd->limits.maxPerStageDescriptorUniformBuffers);
  verify(poolSizes[1].descriptorCount <= vkd->limits.maxPerStageDescriptorSampledImages);
  verify((poolSizes[0].descriptorCount + poolSizes[1].descriptorCount) <=
         vkd->limits.maxPerStageResources);

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2;
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.maxSets = maxAllocatedDescriptorSetsCount;
  // TODO check bound VULKAN_ASSERT(poolInfo.maxSets <= vkd.limits.maxBoundDescriptorSets);

  VkDescriptorPool descriptorPool;
  verify(vkCreateDescriptorPool(vkd->device, &poolInfo, vka, &descriptorPool) == VK_SUCCESS);
  return descriptorPool;
}

VkDescriptorSetLayout create_descriptor_set_layout(vulkan_device *vkd,
                                                   VkDescriptorType descriptorType,
                                                   uint32_t descriptorCount,
                                                   VkShaderStageFlags stageFlags) {
  VkDescriptorSetLayoutBinding layoutBinding = {0};
  layoutBinding.binding = 0;
  layoutBinding.descriptorCount = descriptorCount;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.pImmutableSamplers = NULL;
  layoutBinding.stageFlags = stageFlags;

  VkDescriptorSetLayoutBinding bindings[1] = {layoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = bindings;

  VkDescriptorSetLayout descriptorSetLayout;
  verify(vkCreateDescriptorSetLayout(vkd->device, &layoutInfo, vka, &descriptorSetLayout) ==
         VK_SUCCESS);
  return descriptorSetLayout;
}

VkDescriptorSet create_descriptor_set_for_uniform_buffers(
    vulkan_device *vkd, VkBuffer *uniformBuffers, VkDeviceSize bufferSize, size_t bufferCount,
    VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool) {
  VkDescriptorSetLayout layouts[1] = {descriptorSetLayout};
  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  verify(vkAllocateDescriptorSets(vkd->device, &allocInfo, &descriptorSet) == VK_SUCCESS);

  VkWriteDescriptorSet descriptorWrites[1] = {0};
  VkDescriptorBufferInfo bufferInfo[bufferCount];
  for (uint32_t i = 0; i < bufferCount; i++) {
    bufferInfo[i].buffer = uniformBuffers[i];
    bufferInfo[i].offset = 0;
    bufferInfo[i].range = bufferSize;
  }
  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].dstSet = descriptorSet;
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = bufferCount;
  descriptorWrites[0].pBufferInfo = bufferInfo;

  vkUpdateDescriptorSets(vkd->device, 1, descriptorWrites, 0, NULL);

  return descriptorSet;
}

#include "functions.h"
#include "device.h"

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



#define DEBUG_NAME_FORMAT_START()                                                                  \
  va_list args;                                                                                    \
  va_start(args, debugFormat);                                                                     \
  char *debugName;                                                                                 \
  UT_string *s;                                                                                    \
  utstring_new(s);                                                                                 \
  utstring_printf_va(s, debugFormat, args);                                                        \
  va_end(args);                                                                                    \
  debugName = strdup(utstring_body(s));                                                            \
  utstring_free(s);

#define DEBUG_NAME_FORMAT_END() free(debugName);

VkCommandPool vulkan_create_command_pool(vulkan_device *vkd, uint32_t queueFamilyIndex,
                                         VkCommandPoolCreateFlags flags, const char *debugFormat,
                                         ...) {
  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndex;
  poolInfo.flags = flags;

  VkCommandPool commandPool;
  verify(vkCreateCommandPool(vkd->device, &poolInfo, vka, &commandPool) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_command_pool(vkd->debug, commandPool, "%s - command pool", debugName);
  DEBUG_NAME_FORMAT_END();

  return commandPool;
}

VkCommandBuffer vulkan_create_command_buffer(vulkan_device *vkd, VkCommandPool commandPool,
                                             const char *debugFormat, ...) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  verify(vkAllocateCommandBuffers(vkd->device, &allocInfo, &commandBuffer) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_command_buffer(vkd->debug, commandBuffer, "%s - command buffer", debugName);
  DEBUG_NAME_FORMAT_END();

  return commandBuffer;
}

VkFramebuffer vulkan_create_framebuffer(vulkan_device *vkd, VkRenderPass renderPass,
                                        uint32_t attachmentCount, const VkImageView *attachments,
                                        uint32_t width, uint32_t height, const char *debugFormat,
                                        ...) {
  VkFramebufferCreateInfo framebufferInfo = {0};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments;
  framebufferInfo.width = width;
  framebufferInfo.height = height;
  framebufferInfo.layers = 1; // NOTE: 1 layer when rendering to swap chain image, could use more
                              // layers if rendering to shadow maps, VR etc.

  VkFramebuffer framebuffer;
  verify(vkCreateFramebuffer(vkd->device, &framebufferInfo, vka, &framebuffer) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_framebuffer(vkd->debug, framebuffer, "%s - framebuffer", debugName);
  DEBUG_NAME_FORMAT_END();

  return framebuffer;
}

void vulkan_create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                         uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                         VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage *image,
                         VkDeviceMemory *imageMemory, const char *debugFormat, ...) {
  assert(0);
}

VkImageView vulkan_create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                                     VkFormat format, VkImageAspectFlags aspectFlags,
                                     uint32_t mipLevels, uint32_t arrayLayers,
                                     const char *debugFormat, ...) {
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

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_image_view(vkd->debug, imageView, "%s - image view (%s)", debugName,
                               VkImageViewType_debug_str(viewInfo.viewType));
  DEBUG_NAME_FORMAT_END();

  return imageView;
}

void vulkan_create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer *buffer,
                          VkDeviceMemory *bufferMemory, const char *debugFormat, ...) {
  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  DEBUG_NAME_FORMAT_START();
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
  DEBUG_NAME_FORMAT_END();

  vkBindBufferMemory(vkd->device, *buffer, *bufferMemory, 0);
}

VkShaderModule vulkan_create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size,
                                           const char *debugFormat, ...) {
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size * sizeof(char);
  createInfo.pCode = code;

  VkShaderModule shaderModule;
  verify(vkCreateShaderModule(vkd->device, &createInfo, vka, &shaderModule) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_shader_module(vkd->debug, shaderModule, "%s - shader module", debugName);
  DEBUG_NAME_FORMAT_END();

  return shaderModule;
}

VkDescriptorPool vulkan_create_descriptor_pool(vulkan_device *vkd, size_t totalUniformBufferCount,
                                               size_t totalCombinedImageSamplerCount,
                                               size_t maxAllocatedDescriptorSetsCount,
                                               const char *debugFormat, ...) {
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

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_descriptor_pool(
      vkd->debug, descriptorPool,
      "%s - descriptor pool (%zu uniforms, %zu combined, max %zu descriptor sets)", debugName,
      totalUniformBufferCount, totalCombinedImageSamplerCount, maxAllocatedDescriptorSetsCount);
  DEBUG_NAME_FORMAT_END();

  return descriptorPool;
}

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(vulkan_device *vkd,
                                                          VkDescriptorType descriptorType,
                                                          uint32_t descriptorCount,
                                                          VkShaderStageFlags stageFlags,
                                                          const char *debugFormat, ...) {
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

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_descriptor_set_layout(vkd->debug, descriptorSetLayout,
                                          "%s - descriptor set layout (%zu bindings)", debugName,
                                          layoutInfo.bindingCount);
  DEBUG_NAME_FORMAT_END();

  return descriptorSetLayout;
}

VkDescriptorSet vulkan_create_descriptor_set_for_uniform_buffers(
    vulkan_device *vkd, VkBuffer *uniformBuffers, VkDeviceSize bufferSize, size_t bufferCount,
    VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
    const char *debugFormat, ...) {
  VkDescriptorSetLayout layouts[1] = {descriptorSetLayout};
  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  verify(vkAllocateDescriptorSets(vkd->device, &allocInfo, &descriptorSet) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_descriptor_set(vkd->debug, descriptorSet, "%s - descriptor set", debugName);
  DEBUG_NAME_FORMAT_END();

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

VkSemaphore vulkan_create_semaphore(vulkan_device *vkd, VkSemaphoreCreateFlags flags,
                                    const char *debugFormat, ...) {
  VkSemaphoreCreateInfo semaphoreInfo = {0};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.flags = flags;

  VkSemaphore semaphore;
  verify(vkCreateSemaphore(vkd->device, &semaphoreInfo, vka, &semaphore) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_semaphore(vkd->debug, semaphore, "%s - semaphore", debugName);
  DEBUG_NAME_FORMAT_END();

  return semaphore;
}

VkFence vulkan_create_fence(vulkan_device *vkd, VkFenceCreateFlags flags, const char *debugFormat,
                            ...) {
  VkFenceCreateInfo fenceInfo = {0};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = flags;

  VkFence fence;
  verify(vkCreateFence(vkd->device, &fenceInfo, vka, &fence) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_fence(vkd->debug, fence, "%s - fence", debugName);
  DEBUG_NAME_FORMAT_END();

  return fence;
}

#undef DEBUG_NAME_FORMAT_START
#undef DEBUG_NAME_FORMAT_END

VkCommandBuffer vulkan_begin_one_shot_commands(vulkan_device *vkd) {
  VkCommandBuffer commandBuffer;
  commandBuffer = vulkan_create_command_buffer(vkd, vkd->oneShotCommandPool, "one-shot");

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void vulkan_end_one_shot_commands(vulkan_device *vkd, VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(vkd->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(vkd->graphicsQueue);

  vkFreeCommandBuffers(vkd->device, vkd->oneShotCommandPool, 1, &commandBuffer);
}

void vulkan_copy_buffer_to_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                  VkDeviceSize size) {
  VkCommandBuffer commandBuffer = vulkan_begin_one_shot_commands(vkd);

  VkBufferCopy copyRegion = {0};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  vulkan_end_one_shot_commands(vkd, commandBuffer);
}

void vulkan_copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image, uint32_t width,
                                 uint32_t height, uint32_t baseArrayLayer) {
  assert(0);
}

void vulkan_generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat,
                             int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
  assert(0);
}

void vulkan_transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                                    VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t mipLevels, uint32_t arrayLayers) {
  assert(0);
}

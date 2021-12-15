#include "functions.h"
#include "../codegen/functions.c"

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

void create_image(vulkan_device *vkd, uint32_t width, uint32_t height,
                  uint32_t mipLevels, uint32_t arrayLayers,
                  VkSampleCountFlagBits numSamples, VkFormat format,
                  VkImageTiling tiling, VkImageCreateFlags flags,
                  VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkImage image, VkDeviceMemory imageMemory) {}

VkImageView create_image_view(vulkan_device *vkd, VkImage image,
                              VkImageViewType type, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels, uint32_t arrayLayers) {
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

void copy_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer commandBuffer = begin_single_time_commands(vkd);

  VkBufferCopy copyRegion = {0};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  end_single_time_commands(vkd, commandBuffer);
}

void copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image,
                          uint32_t width, uint32_t height,
                          uint32_t baseArrayLayer) {}

void generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat,
                      int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
}

void transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels, uint32_t arrayLayers) {}

VkShaderModule create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size) {
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size * sizeof(char);
  createInfo.pCode = code;

  VkShaderModule shaderModule;
  verify(vkCreateShaderModule(vkd->device, &createInfo, vka, &shaderModule) == VK_SUCCESS);
  return shaderModule;
}

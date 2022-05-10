#include "functions.h"

#include "assets/texture.h"
#include "objects/buffer.h"
#include "objects/device.h"
#include "objects/image.h"
#include "objects/shader_program.h"
#include "objects/swap_chain.h"

uint32_t vulkan_find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
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

VkFormat vulkan_find_supported_format(vulkan_device *vkd, VkImageTiling tiling,
                                      VkFormatFeatureFlags features, VkFormat *candidates,
                                      size_t candidateCount) {
  assert(candidateCount > 0);

  for (size_t i = 0; i < candidateCount; i++) {
    VkFormat format = candidates[i];

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vkd->physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  for (size_t i = 0; i < candidateCount; i++) {
    VkFormat format = candidates[i];
    log_error("unsupported format %s", VkFormat_debug_str(format));
  }
  panic("unsupported format");
  return VK_FORMAT_UNDEFINED;
}

VkFormat vulkan_find_depth_format(vulkan_device *vkd) {
  VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_FORMAT_D24_UNORM_S8_UINT};
  return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, formats,
                                      array_size(formats));
}

VkFormat vulkan_find_texture_format(vulkan_device *vkd, vulkan_asset_texture *texture) {
  size_t channels = texture->image->channels;
  assert(channels > 0 && channels <= 4);
  bool sRGB = texture->image->type == vulkan_image_type_material_base_color;

  VkFormat r8 = sRGB ? VK_FORMAT_R8_SRGB : VK_FORMAT_R8_UNORM;
  VkFormat r8g8 = sRGB ? VK_FORMAT_R8G8_SRGB : VK_FORMAT_R8G8_UNORM;
  VkFormat r8g8b8 = sRGB ? VK_FORMAT_R8G8B8_SRGB : VK_FORMAT_R8G8B8_UNORM;
  VkFormat r8g8b8a8 = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

  if (channels == 1) {
    VkFormat formats[] = {r8, r8g8, r8g8b8, r8g8b8a8};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 2) {
    VkFormat formats[] = {r8g8, r8g8b8, r8g8b8a8};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 3) {
    VkFormat formats[] = {r8g8b8, r8g8b8a8};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 4) {
    VkFormat formats[] = {r8g8b8a8};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  assert(0);
  return VK_FORMAT_UNDEFINED;
}

size_t vulkan_format_size(VkFormat format) {
  if (format == VK_FORMAT_R8_SRGB || format == VK_FORMAT_R8_UNORM) {
    return 1;
  }
  if (format == VK_FORMAT_R8G8_SRGB || format == VK_FORMAT_R8G8_UNORM) {
    return 2;
  }
  if (format == VK_FORMAT_R8G8B8_SRGB || format == VK_FORMAT_R8G8B8_UNORM) {
    return 3;
  }
  if (format == VK_FORMAT_R8G8B8A8_SRGB || format == VK_FORMAT_R8G8B8A8_UNORM) {
    return 4;
  }
  assert(0);
  return 0;
}

VkIndexType vulkan_stride_to_index_type(size_t stride) {
  if (stride == 2) {
    return VK_INDEX_TYPE_UINT16;
  }
  if (stride == 4) {
    return VK_INDEX_TYPE_UINT32;
  }
  assert(0);
  return 0;
}

#define DEBUG_NAME_FORMAT_START()                                                                  \
  va_list args;                                                                                    \
  va_start(args, debugFormat);                                                                     \
  char *debugName;                                                                                 \
  UT_string *s;                                                                                    \
  utstring_new(s);                                                                                 \
  utstring_printf_va(s, debugFormat, args);                                                        \
  va_end(args);                                                                                    \
  debugName = core_strdup(utstring_body(s));                                                       \
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
  assert(renderPass != VK_NULL_HANDLE);

  VkFramebufferCreateInfo framebufferInfo = {0};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = attachmentCount;
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

VkImage vulkan_create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                            uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                            VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                            const char *debugFormat, ...) {
  VkImageCreateInfo imageInfo = {0};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.flags = flags;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = arrayLayers;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = numSamples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkImage image;
  verify(vkCreateImage(vkd->device, &imageInfo, vka, &image) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_image(vkd->debug, image, "%s - image (%s, %s)", debugName,
                          VkImageViewType_debug_str(imageInfo.imageType),
                          VkFormat_debug_str(imageInfo.format));
  DEBUG_NAME_FORMAT_END();

  return image;
}

VkDeviceMemory vulkan_create_image_memory(vulkan_device *vkd, VkImage image,
                                          VkMemoryPropertyFlags properties, const char *debugFormat,
                                          ...) {
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(vkd->device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      vulkan_find_memory_type(vkd, memRequirements.memoryTypeBits, properties);

  VkDeviceMemory imageMemory;
  verify(vkAllocateMemory(vkd->device, &allocInfo, vka, &imageMemory) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_device_memory(vkd->debug, imageMemory, "%s - image memory (%zu bytes)",
                                  debugName, allocInfo.allocationSize);
  DEBUG_NAME_FORMAT_END();

  vkBindImageMemory(vkd->device, image, imageMemory, 0);

  return imageMemory;
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

VkSampler vulkan_create_sampler(vulkan_device *vkd, uint32_t mipLevelCount, const char *debugFormat,
                                ...) {
  // TODO: Share sampler with same parameters.
  VkSamplerCreateInfo samplerInfo = {0};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = (float)(mipLevelCount);
  samplerInfo.mipLodBias = 0.0f;

  VkSampler sampler;
  verify(vkCreateSampler(vkd->device, &samplerInfo, vka, &sampler) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  vulkan_debug_name_sampler(vkd->debug, sampler, "%s - sampler (maxLod = %f)", debugName,
                            samplerInfo.maxLod);
  DEBUG_NAME_FORMAT_END();

  return sampler;
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
  allocInfo.memoryTypeIndex =
      vulkan_find_memory_type(vkd, memRequirements.memoryTypeBits, properties);
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
                                               bool bindless, const char *debugFormat, ...) {
  VkDescriptorPoolSize poolSizes[2] = {0};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = totalUniformBufferCount;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = totalCombinedImageSamplerCount;

  if (!bindless) {
    verify(poolSizes[0].descriptorCount <= vkd->limits.maxPerStageDescriptorUniformBuffers);
    verify(poolSizes[1].descriptorCount <= vkd->limits.maxPerStageDescriptorSampledImages);
    verify((poolSizes[0].descriptorCount + poolSizes[1].descriptorCount) <=
           vkd->limits.maxPerStageResources);
  } else {
    verify(poolSizes[0].descriptorCount <= vkd->limits.maxPerStageBindlessDescriptorUniformBuffers);
    verify(poolSizes[1].descriptorCount <= vkd->limits.maxPerStageBindlessDescriptorSampledImages);
    verify((poolSizes[0].descriptorCount + poolSizes[1].descriptorCount) <=
           vkd->limits.maxPerStageBindlessResources);
  }

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = bindless ? VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT : 0;
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
                                                          vulkan_descriptor_binding *bindings,
                                                          size_t bindingCount, bool bindless,
                                                          const char *debugFormat, ...) {

  VkDescriptorSetLayoutBinding layoutBindings[bindingCount];
  for (size_t i = 0; i < bindingCount; i++) {
    VkDescriptorSetLayoutBinding *layoutBinding = &layoutBindings[i];
    vulkan_descriptor_binding *binding = &bindings[i];

    *layoutBinding = (VkDescriptorSetLayoutBinding){.binding = binding->bindingNumber,
                                                    .descriptorCount = binding->descriptorCount,
                                                    .descriptorType = binding->descriptorType,
                                                    .pImmutableSamplers = NULL,
                                                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                                                                  VK_SHADER_STAGE_FRAGMENT_BIT};
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = bindingCount;
  layoutInfo.pBindings = layoutBindings;
  VkDescriptorSetLayoutBindingFlagsCreateInfo layoutBindingFlagsInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO};
  VkDescriptorBindingFlags layoutBindingFlags[layoutInfo.bindingCount];
  if (bindless) {
    layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    for (size_t i = 0; i < array_size(layoutBindingFlags); i++) {
      VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
      layoutBindingFlags[i] = bindingFlags;
    }
    layoutBindingFlags[array_size(layoutBindingFlags) - 1] |=
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    layoutBindingFlagsInfo.bindingCount = layoutInfo.bindingCount;
    layoutBindingFlagsInfo.pBindingFlags = layoutBindingFlags;
    layoutInfo.pNext = &layoutBindingFlagsInfo;
  }

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

VkDescriptorSet
vulkan_create_descriptor_set(vulkan_device *vkd, VkDescriptorSetLayout descriptorSetLayout,
                             VkDescriptorPool descriptorPool, vulkan_descriptor_binding *bindings,
                             size_t bindingCount, bool bindless, const char *debugFormat, ...) {

  VkDescriptorSetLayout layouts[1] = {descriptorSetLayout};
  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;
  VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO};
  uint32_t maxBindlessDescriptors = vkd->limits.maxPerStageBindlessDescriptorSampledImages;
  if (bindless) {
    variableDescriptorCountAllocateInfo.descriptorSetCount = 1;
    // NOTE: Assumed that variable-sized descriptor binds images.
    variableDescriptorCountAllocateInfo.pDescriptorCounts = &maxBindlessDescriptors;
    allocInfo.pNext = &variableDescriptorCountAllocateInfo;
  }

  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  verify(vkAllocateDescriptorSets(vkd->device, &allocInfo, &descriptorSet) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_descriptor_set(vkd->debug, descriptorSet, "%s - descriptor set", debugName);
  DEBUG_NAME_FORMAT_END();

  vulkan_update_descriptor_set(vkd, descriptorSet, bindings, bindingCount);

  return descriptorSet;
}

void vulkan_update_descriptor_set(vulkan_device *vkd, VkDescriptorSet descriptorSet,
                                  vulkan_descriptor_binding *bindings, size_t bindingCount) {
  VkWriteDescriptorSet descriptorWrites[bindingCount];
  VkDescriptorBufferInfo bufferInfos[bindingCount];
  size_t unifiedBufferWrites = 0;
  for (size_t i = 0; i < bindingCount; i++) {
    vulkan_descriptor_binding *binding = &bindings[i];

    if (binding->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
      VkWriteDescriptorSet *descriptorWrite = &descriptorWrites[unifiedBufferWrites];

      *descriptorWrite = (VkWriteDescriptorSet){.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                                .dstSet = descriptorSet,
                                                .dstBinding = binding->bindingNumber,
                                                .dstArrayElement = 0,
                                                .descriptorType = binding->descriptorType,
                                                .descriptorCount = binding->descriptorCount};

      assert(binding->descriptorCount == 1);
      VkDescriptorBufferInfo *bufferInfo = &bufferInfos[unifiedBufferWrites];
      bufferInfo->buffer = binding->bufferElement->buffer->buffer;
      bufferInfo->offset = binding->bufferElement->bufferOffset;
      assert(bufferInfo->offset % vkd->limits.minUniformBufferOffsetAlignment == 0);
      bufferInfo->range = binding->bufferElement->size;
      assert(bufferInfo->range <= vkd->limits.maxUniformBufferRange);
      descriptorWrite->pBufferInfo = bufferInfo;

      unifiedBufferWrites++;
    }
  }
  if (unifiedBufferWrites > 0) {
    vkUpdateDescriptorSets(vkd->device, unifiedBufferWrites, descriptorWrites, 0, NULL);
  }

  for (size_t i = 0; i < bindingCount; i++) {
    vulkan_descriptor_binding *binding = &bindings[i];
    if (binding->descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
      vulkan_textures *textures = binding->textures;
      uint32_t textureElementCount = HASH_COUNT(textures->textureElements);
      if (textureElementCount == 0) {
        continue;
      }
      VkWriteDescriptorSet texturesDescriptorWrites[textureElementCount];
      VkDescriptorImageInfo imageInfos[textureElementCount];
      size_t texturesDescriptorWriteIdx = 0;
      for (vulkan_textures_texture_element *textureElement = textures->textureElements;
           textureElement != NULL; textureElement = textureElement->hh.next) {
        VkWriteDescriptorSet *descriptorWrite =
            &texturesDescriptorWrites[texturesDescriptorWriteIdx];

        *descriptorWrite = (VkWriteDescriptorSet){.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                                  .dstSet = descriptorSet,
                                                  .dstBinding = binding->bindingNumber,
                                                  .dstArrayElement = textureElement->textureIdx,
                                                  .descriptorType = binding->descriptorType,
                                                  .descriptorCount = 1};

        VkDescriptorImageInfo *imageInfo = &imageInfos[texturesDescriptorWriteIdx];
        imageInfo->sampler = textureElement->sampler;
        imageInfo->imageView = textureElement->image->imageView;
        imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorWrite->pImageInfo = imageInfo;

        texturesDescriptorWriteIdx++;
      }
      vkUpdateDescriptorSets(vkd->device, textureElementCount, texturesDescriptorWrites, 0, NULL);
    }
  }
}

VkPipelineLayout vulkan_create_pipeline_layout(vulkan_device *vkd,
                                               VkPipelineLayoutCreateFlags flags,
                                               const VkDescriptorSetLayout *descriptorSetLayouts,
                                               size_t descriptorSetLayoutCount,
                                               const VkPushConstantRange *pushConstantRanges,
                                               size_t pushConstantRangeCount,
                                               const char *debugFormat, ...) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutCount;
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
  pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
  pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  verify(vkCreatePipelineLayout(vkd->device, &pipelineLayoutInfo, vka, &pipelineLayout) ==
         VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_pipeline_layout(vkd->debug, pipelineLayout, "%s - pipeline layout", debugName);
  DEBUG_NAME_FORMAT_END();

  return pipelineLayout;
}

VkPipeline vulkan_create_graphics_pipeline(
    vulkan_device *vkd, vulkan_shader_program *shaderProgram, vulkan_render_state *renderState,
    vulkan_swap_chain *vks,

    const VkDescriptorSetLayout *descriptorSetLayouts, size_t descriptorSetLayoutCount,
    const VkPushConstantRange *pushConstantRanges, size_t pushConstantRangeCount,

    VkRenderPass renderPass, VkPipelineLayout *pipelineLayout, const char *debugFormat, ...) {

  /* shaders */
  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_shader_program_get_shader_stages(shaderProgram, &shaderStageCount);

  /* vertex input */
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  size_t vertexBindingDescriptionsCount =
      vulkan_vertex_stream_get_vertex_buffer_binding_count(renderState->vertexStream);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_vertex_stream_get_vertex_buffer_binding_description(renderState->vertexStream);
  vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionsCount;
  vertexInputInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
  size_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_vertex_stream_get_vertex_attribute_descriptions(renderState->vertexStream,
                                                             &vertexAttributeDescriptionsCount);
  vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionsCount;
  vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  /* viewport */
  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)vks->swapChainExtent.width;
  viewport.height = (float)vks->swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = vks->swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState = {0};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  /* rasterization */
  VkPipelineRasterizationStateCreateInfo rasterizer = {0};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {0};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = true;
  depthStencil.depthWriteEnable = true;
  depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; // NOTE: Reverse Z.
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {0};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  /* pipeline layout */
  DEBUG_NAME_FORMAT_START()
  *pipelineLayout =
      vulkan_create_pipeline_layout(vkd, 0, descriptorSetLayouts, descriptorSetLayoutCount,
                                    pushConstantRanges, pushConstantRangeCount, "%s", debugName);

  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = shaderStageCount;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = *pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  VkPipeline graphicsPipeline;
  verify(vkCreateGraphicsPipelines(vkd->device, VK_NULL_HANDLE, 1, &pipelineInfo, vka,
                                   &graphicsPipeline) == VK_SUCCESS);
  vulkan_debug_name_render_pass(vkd->debug, renderPass, "%s - graphics pipeline", debugName);
  DEBUG_NAME_FORMAT_END();

  core_free(vertexAttributeDescriptions);
  core_free(shaderStages);
  return graphicsPipeline;
}

VkRenderPass vulkan_create_render_pass(
    vulkan_device *vkd, VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    size_t offscreenAttachmentDescriptionCount,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    size_t offscreenAttachmentReferenceCount, VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference, const char *debugFormat, ...) {
  assert(offscreenAttachmentDescriptionCount == offscreenAttachmentReferenceCount);
  size_t onscreenColorAttachmentCount = (onscreenColorAttachmentReference != NULL ? 1 : 0);
  size_t depthAttachmentDescriptionCount = (depthAttachmentDescription != NULL ? 1 : 0);

  size_t attachmentDescriptionCount = onscreenColorAttachmentCount +
                                      offscreenAttachmentDescriptionCount +
                                      depthAttachmentDescriptionCount;
  VkAttachmentDescription attachmentDescriptions[attachmentDescriptionCount];
  size_t idx = 0;
  if (onscreenColorAttachmentCount > 0) {
    attachmentDescriptions[idx++] = *onscreenColorAttachmentDescription;
  }
  core_memcpy(attachmentDescriptions + idx, offscreenColorAttachmentDescriptions,
              offscreenAttachmentDescriptionCount * sizeof(VkAttachmentDescription));
  idx += offscreenAttachmentReferenceCount;
  if (depthAttachmentDescriptionCount > 0) {
    attachmentDescriptions[idx++] = *depthAttachmentDescription;
  }

  size_t colorAttachmentReferenceCount =
      onscreenColorAttachmentCount + offscreenAttachmentReferenceCount;
  VkAttachmentReference colorAttachmentReferences[attachmentDescriptionCount];
  idx = 0;
  if (onscreenColorAttachmentCount > 0) {
    colorAttachmentReferences[idx++] = *onscreenColorAttachmentReference;
  }
  core_memcpy(colorAttachmentReferences + idx, offscreenColorAttachmentReferences,
              offscreenAttachmentReferenceCount * sizeof(VkAttachmentReference));

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = colorAttachmentReferenceCount;
  subpass.pColorAttachments = colorAttachmentReferences;
  subpass.pDepthStencilAttachment = depthAttachmentReference;
  subpass.pResolveAttachments = NULL;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
  // TODO: Better subpass dependency.
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  /*
  dependency.srcStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask =
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  */
  dependency.dependencyFlags = 0;

  VkRenderPassCreateInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = attachmentDescriptionCount;
  renderPassInfo.pAttachments = attachmentDescriptions;

  renderPassInfo.subpassCount = 1; // TODO: Support multiple subpasses.
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkRenderPass renderPass;
  verify(vkCreateRenderPass(vkd->device, &renderPassInfo, vka, &renderPass) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  vulkan_debug_name_render_pass(vkd->debug, renderPass, "%s - render pass", debugName);
  DEBUG_NAME_FORMAT_END();

  return renderPass;
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
  VkCommandBuffer commandBuffer = vulkan_begin_one_shot_commands(vkd);

  VkBufferImageCopy copyRegion = {.bufferOffset = 0,
                                  .bufferRowLength = 0,
                                  .bufferImageHeight = 0,
                                  .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .imageSubresource.mipLevel = 0,
                                  .imageSubresource.baseArrayLayer = baseArrayLayer,
                                  .imageSubresource.layerCount = 1,
                                  .imageOffset = {0, 0, 0},
                                  .imageExtent = {width, height, 1}};
  vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &copyRegion);

  vulkan_end_one_shot_commands(vkd, commandBuffer);
}

void vulkan_generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat format, uint32_t width,
                             uint32_t height, uint32_t mipLevelCount) {

  // Format needs to support linear filtering (vkCmdBlitImage)
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(vkd->physicalDevice, format, &formatProperties);
  verify((formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0);

  VkCommandBuffer commandBuffer = vulkan_begin_one_shot_commands(vkd);

  VkImageMemoryBarrier barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                  .image = image,
                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .subresourceRange.baseArrayLayer = 0,
                                  .subresourceRange.layerCount = 1,
                                  .subresourceRange.levelCount = 1};

  int32_t mipWidth = width;
  int32_t mipHeight = height;
  for (uint32_t i = 1; i < mipLevelCount; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

    VkImageBlit blit = {
        .srcOffsets = {{0, 0, 0}, {mipWidth, mipHeight, 1}},
        .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .srcSubresource.mipLevel = i - 1,
        .srcSubresource.baseArrayLayer = 0,
        .srcSubresource.layerCount = 1,
        .dstOffsets = {{0, 0, 0},
                       {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1}},
        .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .dstSubresource.mipLevel = i,
        .dstSubresource.baseArrayLayer = 0,
        .dstSubresource.layerCount = 1};
    vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }
    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  barrier.subresourceRange.baseMipLevel = mipLevelCount - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

  vulkan_end_one_shot_commands(vkd, commandBuffer);
}

void vulkan_transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                                    VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t mipLevels, uint32_t arrayLayers) {
  VkCommandBuffer commandBuffer = vulkan_begin_one_shot_commands(vkd);

  VkImageMemoryBarrier barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                  .oldLayout = oldLayout,
                                  .newLayout = newLayout,
                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .image = image,
                                  .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .subresourceRange.baseMipLevel = 0,
                                  .subresourceRange.levelCount = mipLevels,
                                  .subresourceRange.baseArrayLayer = 0,
                                  .subresourceRange.layerCount = arrayLayers};

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    assert(0);
    return;
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1,
                       &barrier);

  vulkan_end_one_shot_commands(vkd, commandBuffer);
}

#include "device_functions.h"

#include "device.h"

uint32_t find_memory_type(device *vkd, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

VkFormat find_supported_format(device *vkd, VkImageTiling tiling, VkFormatFeatureFlags features,
                               VkFormat *candidates, size_t candidateCount) {
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

size_t format_size(VkFormat format) {
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
  UNREACHABLE;
}

VkIndexType stride_to_index_type(size_t stride) {
  if (stride == 2) {
    return VK_INDEX_TYPE_UINT16;
  }
  if (stride == 4) {
    return VK_INDEX_TYPE_UINT32;
  }
  UNREACHABLE;
}

VkCommandPool create_command_pool(device *vkd, uint32_t queueFamilyIndex,
                                  VkCommandPoolCreateFlags flags, const char *debugFormat, ...) {
  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndex;
  poolInfo.flags = flags;

  VkCommandPool commandPool;
  verify(vkCreateCommandPool(vkd->device, &poolInfo, vka, &commandPool) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  debug_name_command_pool(vkd->debug, commandPool, "%s - command pool", debugName);
  DEBUG_NAME_FORMAT_END();

  return commandPool;
}

VkCommandBuffer create_command_buffer(device *vkd, VkCommandPool commandPool,
                                      const char *debugFormat, ...) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  verify(vkAllocateCommandBuffers(vkd->device, &allocInfo, &commandBuffer) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  debug_name_command_buffer(vkd->debug, commandBuffer, "%s - command buffer", debugName);
  DEBUG_NAME_FORMAT_END();

  return commandBuffer;
}

VkFramebuffer create_framebuffer(device *vkd, VkRenderPass renderPass, uint32_t attachmentCount,
                                 const VkImageView *attachments, uint32_t width, uint32_t height,
                                 const char *debugFormat, ...) {
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
  debug_name_framebuffer(vkd->debug, framebuffer, "%s - framebuffer", debugName);
  DEBUG_NAME_FORMAT_END();

  return framebuffer;
}

VkImage create_image(device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
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
  debug_name_image(vkd->debug, image, "%s - image (%s, %s)", debugName,
                   VkImageViewType_debug_str(imageInfo.imageType),
                   VkFormat_debug_str(imageInfo.format));
  DEBUG_NAME_FORMAT_END();

  return image;
}

VkDeviceMemory create_image_memory(device *vkd, VkImage image, VkMemoryPropertyFlags properties,
                                   const char *debugFormat, ...) {
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(vkd->device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory_type(vkd, memRequirements.memoryTypeBits, properties);

  VkDeviceMemory imageMemory;
  verify(vkAllocateMemory(vkd->device, &allocInfo, vka, &imageMemory) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START();
  debug_name_device_memory(vkd->debug, imageMemory, "%s - image memory (%zu bytes)", debugName,
                           allocInfo.allocationSize);
  DEBUG_NAME_FORMAT_END();

  vkBindImageMemory(vkd->device, image, imageMemory, 0);

  return imageMemory;
}

VkImageView create_image_view(device *vkd, VkImage image, VkImageViewType type, VkFormat format,
                              VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              uint32_t arrayLayers, const char *debugFormat, ...) {
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
  debug_name_image_view(vkd->debug, imageView, "%s - image view (%s)", debugName,
                        VkImageViewType_debug_str(viewInfo.viewType));
  DEBUG_NAME_FORMAT_END();

  return imageView;
}

VkSampler create_sampler(device *vkd, uint32_t mipLevelCount, const char *debugFormat, ...) {
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
  debug_name_sampler(vkd->debug, sampler, "%s - sampler (maxLod = %f)", debugName,
                     samplerInfo.maxLod);
  DEBUG_NAME_FORMAT_END();

  return sampler;
}

void create_buffer(device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory,
                   const char *debugFormat, ...) {
  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  DEBUG_NAME_FORMAT_START();
  verify(vkCreateBuffer(vkd->device, &bufferInfo, vka, buffer) == VK_SUCCESS);
  debug_name_buffer(vkd->debug, *buffer, "%s - buffer (%zu B)", debugName, bufferInfo.size);

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkd->device, *buffer, &memRequirements);
  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory_type(vkd, memRequirements.memoryTypeBits, properties);
  verify(vkAllocateMemory(vkd->device, &allocInfo, vka, bufferMemory) == VK_SUCCESS);

  debug_name_device_memory(vkd->debug, *bufferMemory, "%s - device memory (%zu B)", debugName,
                           memRequirements.size);
  DEBUG_NAME_FORMAT_END();

  vkBindBufferMemory(vkd->device, *buffer, *bufferMemory, 0);
}

VkShaderModule create_shader_module(device *vkd, const uint32_t *code, size_t size,
                                    const char *debugFormat, ...) {
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size * sizeof(char);
  createInfo.pCode = code;

  VkShaderModule shaderModule;
  verify(vkCreateShaderModule(vkd->device, &createInfo, vka, &shaderModule) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  debug_name_shader_module(vkd->debug, shaderModule, "%s - shader module", debugName);
  DEBUG_NAME_FORMAT_END();

  return shaderModule;
}

VkDescriptorPool create_descriptor_pool(device *vkd, size_t totalUniformBufferCount,
                                        size_t totalCombinedImageSamplerCount,
                                        size_t maxAllocatedDescriptorSetsCount, bool bindless,
                                        const char *debugFormat, ...) {
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
  debug_name_descriptor_pool(
      vkd->debug, descriptorPool,
      "%s - descriptor pool (%zu uniforms, %zu combined, max %zu descriptor sets)", debugName,
      totalUniformBufferCount, totalCombinedImageSamplerCount, maxAllocatedDescriptorSetsCount);
  DEBUG_NAME_FORMAT_END();

  return descriptorPool;
}

VkPipelineLayout create_pipeline_layout(device *vkd, VkPipelineLayoutCreateFlags flags,
                                        const VkDescriptorSetLayout *descriptorSetLayouts,
                                        size_t descriptorSetLayoutCount,
                                        const VkPushConstantRange *pushConstantRanges,
                                        size_t pushConstantRangeCount, const char *debugFormat,
                                        ...) {
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
  debug_name_pipeline_layout(vkd->debug, pipelineLayout, "%s - pipeline layout", debugName);
  DEBUG_NAME_FORMAT_END();

  return pipelineLayout;
}

void rendering_info_init(rendering_info *renderPassInfo) {
  *renderPassInfo = (rendering_info){0};
  utarray_alloc(renderPassInfo->offscreenColorAttachments, sizeof(rendering_attachment_info));
  utarray_alloc(renderPassInfo->preImageLayoutTransition,
                sizeof(rendering_image_layout_transition_info));
  utarray_alloc(renderPassInfo->postImageLayoutTransition,
                sizeof(rendering_image_layout_transition_info));
}

void rendering_info_deinit(rendering_info *renderPassInfo) {
  core_free(renderPassInfo->onscreenColorAttachment);
  utarray_free(renderPassInfo->offscreenColorAttachments);
  core_free(renderPassInfo->depthAttachment);
  utarray_free(renderPassInfo->preImageLayoutTransition);
  utarray_free(renderPassInfo->postImageLayoutTransition);
}

void rendering_info_add_onscreen_color_attachment(rendering_info *renderPassInfo,
                                                  rendering_attachment_info attachmentCreateInfo) {
  assert(renderPassInfo->onscreenColorAttachment == NULL);

  renderPassInfo->onscreenColorAttachment = core_alloc(sizeof(rendering_attachment_info));
  *renderPassInfo->onscreenColorAttachment = attachmentCreateInfo;
}

void rendering_info_add_offscreen_color_attachment(rendering_info *renderPassInfo,
                                                   rendering_attachment_info attachmentCreateInfo) {
  utarray_push_back(renderPassInfo->offscreenColorAttachments, &attachmentCreateInfo);
}

void rendering_info_add_depth_attachment(rendering_info *renderPassInfo,
                                         rendering_attachment_info attachmentCreateInfo) {
  assert(renderPassInfo->depthAttachment == NULL);

  renderPassInfo->depthAttachment = core_alloc(sizeof(rendering_attachment_info));
  *renderPassInfo->depthAttachment = attachmentCreateInfo;
}

void rendering_info_pre_image_layout_transition(
    rendering_info *renderPassInfo,
    rendering_image_layout_transition_info imageLayoutTransitionInfo) {
  utarray_push_back(renderPassInfo->preImageLayoutTransition, &imageLayoutTransitionInfo);
}

void rendering_info_post_image_layout_transition(
    rendering_info *renderPassInfo,
    rendering_image_layout_transition_info imageLayoutTransitionInfo) {
  utarray_push_back(renderPassInfo->postImageLayoutTransition, &imageLayoutTransitionInfo);
}

void add_general_memory_barrier(device *vkd, VkCommandBuffer commandBuffer) {
  VkMemoryBarrier generalMemoryBarrier = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
  };
  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &generalMemoryBarrier, 0, NULL, 0,
                       NULL);
}

void begin_rendering(device *vkd, VkCommandBuffer commandBuffer, rendering_info renderPassInfo) {

  // add_general_memory_barrier(vkd, commandBuffer);
  utarray_foreach_elem_it (rendering_image_layout_transition_info *, imageLayoutTransitionInfo,
                           renderPassInfo.preImageLayoutTransition) {
    transition_image_layout(vkd, commandBuffer, imageLayoutTransitionInfo->image,
                            imageLayoutTransitionInfo->imageAspectFlags,
                            imageLayoutTransitionInfo->oldLayout,
                            imageLayoutTransitionInfo->newLayout);
  }

  uint32_t onscreenColorAttachmentCount = renderPassInfo.onscreenColorAttachment ? 1 : 0;
  uint32_t offscreenColorAttachmentCount = utarray_len(renderPassInfo.offscreenColorAttachments);
  uint32_t colorAttachmentCount = onscreenColorAttachmentCount + offscreenColorAttachmentCount;
  uint32_t depthAttachmentCount = renderPassInfo.depthAttachment ? 1 : 0;

  VkRenderingAttachmentInfoKHR colorAttachments[VLA(colorAttachmentCount)];
  VkRenderingAttachmentInfoKHR depthAttachment;
  if (onscreenColorAttachmentCount > 0) {
    assert(onscreenColorAttachmentCount == 1);
    colorAttachments[0] = (VkRenderingAttachmentInfoKHR){
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = renderPassInfo.onscreenColorAttachment->imageView,
        .imageLayout = renderPassInfo.onscreenColorAttachment->currentLayout,
        .loadOp = renderPassInfo.onscreenColorAttachment->loadOp,
        .storeOp = renderPassInfo.onscreenColorAttachment->storeOp,
        .clearValue = renderPassInfo.onscreenColorAttachment->clearValue,
    };
  }
  for (size_t i = 0; i < offscreenColorAttachmentCount; i++) {
    rendering_attachment_info *attachmentInfo =
        utarray_eltptr(renderPassInfo.offscreenColorAttachments, i);
    colorAttachments[onscreenColorAttachmentCount + i] = (VkRenderingAttachmentInfoKHR){
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = attachmentInfo->imageView,
        .imageLayout = attachmentInfo->currentLayout,
        .loadOp = attachmentInfo->loadOp,
        .storeOp = attachmentInfo->storeOp,
        .clearValue = attachmentInfo->clearValue,
    };
  }
  if (depthAttachmentCount > 0) {
    assert(depthAttachmentCount == 1);
    depthAttachment = (VkRenderingAttachmentInfoKHR){
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = renderPassInfo.depthAttachment->imageView,
        .imageLayout = renderPassInfo.depthAttachment->currentLayout,
        .loadOp = renderPassInfo.depthAttachment->loadOp,
        .storeOp = renderPassInfo.depthAttachment->storeOp,
        .clearValue = renderPassInfo.depthAttachment->clearValue,
    };
  }

  VkRenderingInfoKHR renderingInfo = {.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                                      .renderArea.offset = {0, 0},
                                      .renderArea.extent = vkd->swapChainInfo.extent,
                                      .layerCount = 1,
                                      .colorAttachmentCount = colorAttachmentCount,
                                      .pColorAttachments = colorAttachments,
                                      .pDepthAttachment =
                                          (depthAttachmentCount > 0 ? &depthAttachment : NULL)};
  vkd->cmdBeginRendering(commandBuffer, &renderingInfo);
}

void end_rendering(device *vkd, VkCommandBuffer commandBuffer, rendering_info renderPassInfo) {
  vkd->cmdEndRendering(commandBuffer);

  // add_general_memory_barrier(vkd, commandBuffer);
  utarray_foreach_elem_it (rendering_image_layout_transition_info *, imageLayoutTransitionInfo,
                           renderPassInfo.postImageLayoutTransition) {
    transition_image_layout(vkd, commandBuffer, imageLayoutTransitionInfo->image,
                            imageLayoutTransitionInfo->imageAspectFlags,
                            imageLayoutTransitionInfo->oldLayout,
                            imageLayoutTransitionInfo->newLayout);
  }
}

VkPipeline create_graphics_pipeline(device *vkd, graphics_pipeline_create_info createInfo,
                                    rendering_info renderingInfo, const char *debugFormat, ...) {

  /* vertex input */
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = createInfo.vertexBindingDescriptionsCount;
  vertexInputInfo.pVertexBindingDescriptions = createInfo.vertexInputBindingDescriptions;
  vertexInputInfo.vertexAttributeDescriptionCount = createInfo.vertexAttributeDescriptionsCount;
  vertexInputInfo.pVertexAttributeDescriptions = createInfo.vertexAttributeDescriptions;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  /* viewport */
  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)createInfo.framebufferWidth;
  viewport.height = (float)createInfo.framebufferHeight;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = createInfo.framebufferWidth;
  scissor.extent.height = createInfo.framebufferHeight;

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
  depthStencil.depthTestEnable = createInfo.depthTestEnable;
  depthStencil.depthWriteEnable = createInfo.depthWriteEnable;
  depthStencil.depthCompareOp = createInfo.depthTestOp;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  // Blending options for framebuffer attachment.
  VkPipelineColorBlendAttachmentState colorBlendAttachment[VLA(createInfo.colorAttachmentCount)];
  for (size_t i = 0; i < array_size(colorBlendAttachment); i++) {
    // NOTE: Color write mask is applied even with blending disabled.
    colorBlendAttachment[i] = (VkPipelineColorBlendAttachmentState){
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
    if (createInfo.colorBlendingType == color_blending_type_alpha) {
      colorBlendAttachment[i].blendEnable = VK_TRUE;
      colorBlendAttachment[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      colorBlendAttachment[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      colorBlendAttachment[i].colorBlendOp = VK_BLEND_OP_ADD;
      colorBlendAttachment[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      colorBlendAttachment[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      colorBlendAttachment[i].alphaBlendOp = VK_BLEND_OP_ADD;
    }
  }

  VkPipelineColorBlendStateCreateInfo colorBlending = {0};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = createInfo.colorAttachmentCount;
  colorBlending.pAttachments = colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  /* dynamic rendering */
  uint32_t onscreenColorAttachmentCount = renderingInfo.onscreenColorAttachment ? 1 : 0;
  uint32_t offscreenColorAttachmentCount = utarray_len(renderingInfo.offscreenColorAttachments);
  uint32_t colorAttachmentCount = onscreenColorAttachmentCount + offscreenColorAttachmentCount;
  VkFormat colorAttachmentsFormats[VLA(colorAttachmentCount)];
  if (renderingInfo.onscreenColorAttachment) {
    colorAttachmentsFormats[0] = renderingInfo.onscreenColorAttachment->currentFormat;
  }
  for (size_t i = 0; i < offscreenColorAttachmentCount; i++) {
    rendering_attachment_info *attachmentInfo =
        utarray_eltptr(renderingInfo.offscreenColorAttachments, i);
    colorAttachmentsFormats[onscreenColorAttachmentCount + i] = attachmentInfo->currentFormat;
  }

  VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED;
  if (renderingInfo.depthAttachment) {
    depthAttachmentFormat = renderingInfo.depthAttachment->currentFormat;
  }

  VkPipelineRenderingCreateInfoKHR pipelineRendering = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
      .colorAttachmentCount = colorAttachmentCount,
      .pColorAttachmentFormats = colorAttachmentsFormats,
      .depthAttachmentFormat = depthAttachmentFormat,
  };

  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = &pipelineRendering;
  pipelineInfo.stageCount = createInfo.shaderStageCount;
  pipelineInfo.pStages = createInfo.shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = createInfo.pipelineLayout;
  pipelineInfo.renderPass = createInfo.renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  VkPipeline graphicsPipeline;
  verify(vkCreateGraphicsPipelines(vkd->device, VK_NULL_HANDLE, 1, &pipelineInfo, vka,
                                   &graphicsPipeline) == VK_SUCCESS);
  DEBUG_NAME_FORMAT_START()
  debug_name_graphics_pipeline(vkd->debug, graphicsPipeline, "%s - graphics pipeline", debugName);
  DEBUG_NAME_FORMAT_END();

  return graphicsPipeline;
}
VkSemaphore create_semaphore(device *vkd, VkSemaphoreCreateFlags flags, const char *debugFormat,
                             ...) {
  VkSemaphoreCreateInfo semaphoreInfo = {0};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.flags = flags;

  VkSemaphore semaphore;
  verify(vkCreateSemaphore(vkd->device, &semaphoreInfo, vka, &semaphore) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  debug_name_semaphore(vkd->debug, semaphore, "%s - semaphore", debugName);
  DEBUG_NAME_FORMAT_END();

  return semaphore;
}

VkFence create_fence(device *vkd, VkFenceCreateFlags flags, const char *debugFormat, ...) {
  VkFenceCreateInfo fenceInfo = {0};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = flags;

  VkFence fence;
  verify(vkCreateFence(vkd->device, &fenceInfo, vka, &fence) == VK_SUCCESS);

  DEBUG_NAME_FORMAT_START()
  debug_name_fence(vkd->debug, fence, "%s - fence", debugName);
  DEBUG_NAME_FORMAT_END();

  return fence;
}

#undef DEBUG_NAME_FORMAT_START
#undef DEBUG_NAME_FORMAT_END

VkCommandBuffer begin_one_shot_commands(device *vkd) {
  VkCommandBuffer commandBuffer;
  commandBuffer = create_command_buffer(vkd, vkd->oneShotCommandPool, "one-shot");

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void end_one_shot_commands(device *vkd, VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(vkd->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(vkd->graphicsQueue);

  vkFreeCommandBuffers(vkd->device, vkd->oneShotCommandPool, 1, &commandBuffer);
}

void copy_buffer_to_buffer(device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
  VkCommandBuffer commandBuffer = begin_one_shot_commands(vkd);

  VkBufferCopy copyRegion = {0};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  end_one_shot_commands(vkd, commandBuffer);
}

void copy_buffer_to_image(device *vkd, VkBuffer buffer, VkImage image, uint32_t width,
                          uint32_t height, uint32_t baseArrayLayer) {
  VkCommandBuffer commandBuffer = begin_one_shot_commands(vkd);

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

  end_one_shot_commands(vkd, commandBuffer);
}

void generate_mipmaps(device *vkd, VkImage image, VkFormat format, uint32_t width, uint32_t height,
                      uint32_t mipLevelCount) {

  // Format needs to support linear filtering (vkCmdBlitImage)
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(vkd->physicalDevice, format, &formatProperties);
  verify((formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0);

  VkCommandBuffer commandBuffer = begin_one_shot_commands(vkd);

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

  end_one_shot_commands(vkd, commandBuffer);
}

void transition_image_layout(device *vkd, VkCommandBuffer commandBuffer, VkImage image,
                             VkImageAspectFlags imageAspectFlags, VkImageLayout oldLayout,
                             VkImageLayout newLayout) {

  VkImageMemoryBarrier barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                  .oldLayout = oldLayout,
                                  .newLayout = newLayout,
                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                  .image = image,
                                  .subresourceRange.aspectMask = imageAspectFlags,
                                  .subresourceRange.baseMipLevel = 0,
                                  .subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS,
                                  .subresourceRange.baseArrayLayer = 0,
                                  .subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS};

  VkPipelineStageFlags srcStageMask = 0;
  VkPipelineStageFlags dstStageMask = 0;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
    // memory dependency:
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // execution dependency:
    srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    // memory dependency:
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // execution dependency:
    srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
    // memory dependency: depth writes are read during depth tests of from shader
    barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
    // execution dependency: depth tests are completed before fragment shader
    srcStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    // memory dependency: color attachment writes are read in fragment shader
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    // execution dependency: color attachment output is completed before fragment shader
    srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
    // memory dependency: none
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;
    // execution dependency: color attachment output is completed before fragment shader
    srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
    // memory dependency: none
    // execution dependency: none
    return;
  } else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    // memory dependency: none
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;
    // execution dependency: color attachment output is completed before fragment shader
    srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
    // memory dependency: color attachment writes are read by swap chain
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    // execution dependency: color attachment output is completed before end of command buffer
    srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  } else {
    UNREACHABLE;
  }

  vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &barrier);
}

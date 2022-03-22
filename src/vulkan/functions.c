#include "functions.h"

#include "objects/buffer.h"
#include "objects/device.h"
#include "objects/shader_generator.h"
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

  assert(false);
  return VK_FORMAT_UNDEFINED;
}

VkFormat vulkan_find_depth_format(vulkan_device *vkd) {
  VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                        VK_FORMAT_D24_UNORM_S8_UINT};
  return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, formats,
                                      array_size(formats));
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
                                  debugName, VkImageViewType_debug_str(allocInfo.allocationSize));
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
                                                          vulkan_descriptor_binding *bindings,
                                                          size_t bindingCount,
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

VkDescriptorSet vulkan_create_descriptor_set(vulkan_device *vkd,
                                             VkDescriptorSetLayout descriptorSetLayout,
                                             VkDescriptorPool descriptorPool,
                                             vulkan_descriptor_binding *bindings,
                                             size_t bindingCount, const char *debugFormat, ...) {

  /* create descriptor set */
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

  /* update bindings */
  VkWriteDescriptorSet descriptorWrites[bindingCount];
  VkDescriptorBufferInfo bufferInfos[bindingCount];

  for (size_t i = 0; i < bindingCount; i++) {
    VkWriteDescriptorSet *descriptorWrite = &descriptorWrites[i];
    vulkan_descriptor_binding *binding = &bindings[i];
    assert(binding->descriptorCount == 1);

    *descriptorWrite = (VkWriteDescriptorSet){.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                              .dstSet = descriptorSet,
                                              .dstBinding = binding->bindingNumber,
                                              .dstArrayElement = 0,
                                              .descriptorType = binding->descriptorType,
                                              .descriptorCount = binding->descriptorCount};

    if (descriptorWrite->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
      assert(binding->descriptorCount == 1);
      VkDescriptorBufferInfo *bufferInfo = &bufferInfos[i];
      bufferInfo->buffer = binding->bufferElement->buffer->buffer;
      bufferInfo->offset = binding->bufferElement->bufferOffset;
      bufferInfo->range = binding->bufferElement->size;
      descriptorWrite->pBufferInfo = bufferInfo;
    } else {
      assert(0);
    }
  }
  vkUpdateDescriptorSets(vkd->device, array_size(descriptorWrites), descriptorWrites, 0, NULL);

  return descriptorSet;
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
    vulkan_device *vkd, vulkan_shader_generator *shaderGenerator, vulkan_render_state *renderState,
    vulkan_swap_chain *vks,

    const VkDescriptorSetLayout *descriptorSetLayouts, size_t descriptorSetLayoutCount,
    const VkPushConstantRange *pushConstantRanges, size_t pushConstantRangeCount,

    VkRenderPass renderPass, VkPipelineLayout *pipelineLayout, const char *debugFormat, ...) {

  // TODO: Create using description struct vulkan_pipeline_info.

  /* shaders */
  size_t shaderStageCount;
  VkPipelineShaderStageCreateInfo *shaderStages =
      vulkan_shader_generator_get_shader_stages(shaderGenerator, &shaderStageCount);

  /* vertex input */
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  size_t vertexBindingDescriptionsCount =
      vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_count(renderState->vertexStream);
  VkVertexInputBindingDescription vertexInputBindingDescription =
      vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_description(
          renderState->vertexStream);
  vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionsCount;
  vertexInputInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
  size_t vertexAttributeDescriptionsCount;
  VkVertexInputAttributeDescription *vertexAttributeDescriptions =
      vulkan_interleaved_vertex_stream_get_vertex_attribute_descriptions(
          renderState->vertexStream, &vertexAttributeDescriptionsCount);
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
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
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
    vulkan_device *vkd, VkAttachmentDescription *colorAttachmentDescriptions,
    size_t colorAttachmentDescriptionCount, VkAttachmentReference *colorAttachmentReferences,
    size_t colorAttachmentReferenceCount, VkAttachmentDescription depthAttachmentDescription,
    VkAttachmentReference depthAttachmentReference, const char *debugFormat, ...) {
  assert(colorAttachmentDescriptionCount == colorAttachmentReferenceCount);

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = colorAttachmentReferenceCount;
  subpass.pColorAttachments = colorAttachmentReferences;
  subpass.pDepthStencilAttachment = &depthAttachmentReference;
  subpass.pResolveAttachments = NULL;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#graphics-to-graphics-dependencies
  // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.srcAccessMask = 0;
  // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.srcStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask =
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  dependency.dependencyFlags = 0;

  VkRenderPassCreateInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

  size_t attachmentCount = colorAttachmentDescriptionCount + 1;
  VkAttachmentDescription attachments[attachmentCount];
  core_memcpy(attachments, colorAttachmentDescriptions,
              colorAttachmentDescriptionCount * sizeof(VkAttachmentDescription));
  attachments[attachmentCount - 1] = depthAttachmentDescription;

  renderPassInfo.attachmentCount = attachmentCount;
  renderPassInfo.pAttachments = attachments;

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

#include "descriptor.h"

#include "../constants.h"
#include "textures.h"
#include "unified_uniform_buffer.h"

#define def_uniform_buffer_data(_name, ...)                                                        \
  vulkan_##_name##_uniform_buffer_data *vulkan_##_name##_uniform_buffer_data_create(               \
      uint32_t count, uint32_t frames) {                                                           \
    vulkan_##_name##_uniform_buffer_data *uniformBuffer =                                          \
        core_alloc(sizeof(vulkan_##_name##_uniform_buffer_data) +                                  \
                   count * frames * sizeof(vulkan_##_name##_uniform_buffer_element));              \
    uniformBuffer->bufferElement = (vulkan_buffer_element){0};                                     \
    uniformBuffer->count = count;                                                                  \
    uniformBuffer->frames = frames;                                                                \
    for (size_t idx = 0; idx < count * frames; idx++) {                                            \
      uniformBuffer->elements[idx] = (vulkan_##_name##_uniform_buffer_element){0};                 \
    }                                                                                              \
    return uniformBuffer;                                                                          \
  }                                                                                                \
  void vulkan_##_name##_uniform_buffer_data_destroy(                                               \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer) {                                       \
    core_free(uniformBuffer);                                                                      \
  }                                                                                                \
  size_t vulkan_##_name##_uniform_buffer_data_get_count(                                           \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer) {                                       \
    return uniformBuffer->count * uniformBuffer->frames;                                           \
  }                                                                                                \
  size_t vulkan_##_name##_uniform_buffer_data_get_size(                                            \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer) {                                       \
    return vulkan_##_name##_uniform_buffer_data_get_count(uniformBuffer) *                         \
           sizeof(vulkan_##_name##_uniform_buffer_element);                                        \
  }                                                                                                \
  vulkan_##_name##_uniform_buffer_element *vulkan_##_name##_uniform_buffer_data_get_element(       \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t index,                           \
      size_t currentFrameInFlight) {                                                               \
    assert(currentFrameInFlight < FRAMES_IN_FLIGHT);                                               \
    assert(index < uniformBuffer->count);                                                          \
    size_t idx = FRAMES_IN_FLIGHT * index + currentFrameInFlight;                                  \
    return &uniformBuffer->elements[idx];                                                          \
  }
VULKAN_UNIFORM_BUFFERS(def_uniform_buffer_data, )
#undef def_uniform_buffer_data

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
        // NOTE: Images in textures descriptor should be in either
        //       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL or
        //       VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
        //       depending on image aspect flags.
        if (textureElement->image->aspectFlags == VK_IMAGE_ASPECT_COLOR_BIT) {
          imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else if (textureElement->image->aspectFlags == VK_IMAGE_ASPECT_DEPTH_BIT) {
          imageInfo->imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        } else {
          UNREACHABLE;
        }
        descriptorWrite->pImageInfo = imageInfo;

        texturesDescriptorWriteIdx++;
      }
      vkUpdateDescriptorSets(vkd->device, texturesDescriptorWriteIdx, texturesDescriptorWrites, 0,
                             NULL);
    }
  }
}

void vulkan_descriptor_binding_debug_print(vulkan_descriptor_binding *binding, int indent) {
  log_debug(INDENT_FORMAT_STRING "binding:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "descriptorSetNumber=%zu", INDENT_FORMAT_ARGS(2),
            binding->descriptors->descriptorSetNumber);
  log_debug(INDENT_FORMAT_STRING "bindingNumber=%zu", INDENT_FORMAT_ARGS(2),
            binding->bindingNumber);
  log_debug(INDENT_FORMAT_STRING "descriptorCount=%zu", INDENT_FORMAT_ARGS(2),
            binding->descriptorCount);
  log_debug(INDENT_FORMAT_STRING "descriptorType=%s", INDENT_FORMAT_ARGS(2),
            VkDescriptorType_debug_str(binding->descriptorType));
  if (binding->descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
    vulkan_buffer_element_debug_print(binding->bufferElement, 2);
  }
}

vulkan_descriptors *vulkan_descriptors_create(vulkan_device *vkd,
                                              vulkan_unified_uniform_buffer *unifiedUniformBuffer,
                                              vulkan_textures *textures) {
  vulkan_descriptors *descriptors = core_alloc(sizeof(vulkan_descriptors));

  descriptors->vkd = vkd;
  descriptors->unifiedUniformBuffer = unifiedUniformBuffer;
  descriptors->textures = textures;

  size_t totalUniformBufferDescriptorCount = 0;
#define binding_uniform_buffer(_name, ...) totalUniformBufferDescriptorCount++;
  VULKAN_UNIFORM_BUFFERS(binding_uniform_buffer, )
#undef binding_uniform_buffer
  size_t totalCombinedImageSamplerDescriptorCount =
      descriptors->vkd->limits.maxPerStageBindlessDescriptorSampledImages;
  descriptors->descriptorPool = vulkan_create_descriptor_pool(
      descriptors->vkd, totalUniformBufferDescriptorCount, totalCombinedImageSamplerDescriptorCount,
      1, true, "descriptors");

  descriptors->descriptorSetNumber = 0;

  size_t bindingNum = 0;
  vulkan_descriptor_binding *binding;
  // add uniform buffer bindings
#define binding_uniform_buffer(_name, ...)                                                         \
  binding = &descriptors->uniformBufferBindings[bindingNum];                                       \
  *binding = (vulkan_descriptor_binding){.descriptors = descriptors,                               \
                                         .bindingNumber = bindingNum++,                            \
                                         .descriptorCount = 1,                                     \
                                         .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,      \
                                         .bufferElement =                                          \
                                             &unifiedUniformBuffer->_name##Data->bufferElement};   \
  descriptors->_name##UniformBufferBinding = binding;
  VULKAN_UNIFORM_BUFFERS(binding_uniform_buffer, )
#undef binding_uniform_buffer
  assert(bindingNum == array_size(descriptors->uniformBufferBindings));
  // add array of textures binding

  binding = &descriptors->texturesBinding;
  *binding = (vulkan_descriptor_binding){
      .descriptors = descriptors,
      .bindingNumber = bindingNum++,
      .descriptorCount =
          descriptors->vkd->limits
              .maxPerStageBindlessDescriptorSampledImages, // variable binding description count
                                                           // upper bound
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .textures = descriptors->textures};

  // create descriptors
  size_t bindingCount = bindingNum;
  vulkan_descriptor_binding bindings[bindingCount];
  bindingNum = array_size(descriptors->uniformBufferBindings);
  core_memcpy(bindings, descriptors->uniformBufferBindings,
              bindingNum * sizeof(vulkan_descriptor_binding));
  bindings[bindingNum++] = descriptors->texturesBinding;

  descriptors->descriptorSetLayout = vulkan_create_descriptor_set_layout(
      descriptors->vkd, bindings, array_size(bindings), true, "descriptors");

  descriptors->descriptorSet = vulkan_create_descriptor_set(
      descriptors->vkd, descriptors->descriptorSetLayout, descriptors->descriptorPool, bindings,
      array_size(bindings), true, "descriptors");

  // create pipeline layout
  size_t descriptorSetLayoutCount = 0;
  VkDescriptorSetLayout *descriptorSetLayouts =
      vulkan_descriptors_get_descriptor_set_layouts(descriptors, &descriptorSetLayoutCount);
  assert(descriptorSetLayoutCount > 0);

  size_t pushConstantRangeCount = 1;
  VkPushConstantRange pushConstantRanges[pushConstantRangeCount];
  pushConstantRanges[0] = (VkPushConstantRange){.stageFlags = VK_SHADER_STAGE_ALL,
                                                .offset = 0,
                                                .size = sizeof(vulkan_draw_push_constant_element)};

  descriptors->pipelineLayout =
      vulkan_create_pipeline_layout(vkd, 0, descriptorSetLayouts, descriptorSetLayoutCount,
                                    pushConstantRanges, pushConstantRangeCount, "descriptors");

  core_free(descriptorSetLayouts);

  return descriptors;
}

void vulkan_descriptors_destroy(vulkan_descriptors *descriptors) {
  vkDestroyPipelineLayout(descriptors->vkd->device, descriptors->pipelineLayout, vka);
  vkDestroyDescriptorSetLayout(descriptors->vkd->device, descriptors->descriptorSetLayout, vka);
  vkDestroyDescriptorPool(descriptors->vkd->device, descriptors->descriptorPool, vka);
  core_free(descriptors);
}

void vulkan_descriptors_update(vulkan_descriptors *descriptors) {
  // No-op.
}

void vulkan_descriptors_send_to_device(vulkan_descriptors *descriptors) {
  vulkan_update_descriptor_set(descriptors->vkd, descriptors->descriptorSet,
                               &descriptors->texturesBinding, 1);
}

void vulkan_descriptors_record_bind_commands(vulkan_descriptors *descriptors,
                                             VkCommandBuffer commandBuffer,
                                             vulkan_draw_push_constant_element drawPushConstant) {
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          descriptors->pipelineLayout, 0, 1, &descriptors->descriptorSet, 0, NULL);

  vkCmdPushConstants(commandBuffer, descriptors->pipelineLayout, VK_SHADER_STAGE_ALL, 0,
                     sizeof(drawPushConstant), &drawPushConstant);
}

VkDescriptorSetLayout *
vulkan_descriptors_get_descriptor_set_layouts(vulkan_descriptors *descriptors, size_t *count) {
  size_t descriptorSetLayoutCount = 1;
  *count = descriptorSetLayoutCount;

  VkDescriptorSetLayout *descriptorSetLayouts =
      core_alloc(descriptorSetLayoutCount * sizeof(VkPipelineShaderStageCreateInfo));
  descriptorSetLayouts[0] = descriptors->descriptorSetLayout;

  return descriptorSetLayouts;
}

void vulkan_descriptors_debug_print(vulkan_descriptors *descriptors, int indent) {
  log_debug(INDENT_FORMAT_STRING "descriptors (set=%zu):", INDENT_FORMAT_ARGS(0),
            descriptors->descriptorSetNumber);

#define debug_print_uniform_buffer(_name, ...)                                                     \
  log_debug(INDENT_FORMAT_STRING #_name ":", INDENT_FORMAT_ARGS(2));                               \
  vulkan_descriptor_binding_debug_print(descriptors->_name##UniformBufferBinding, indent + 4);
  VULKAN_UNIFORM_BUFFERS(debug_print_uniform_buffer, )
#undef debug_print_uniform_buffer

  vulkan_descriptor_binding_debug_print(&descriptors->texturesBinding, indent + 4);
}

#include "descriptor.h"

#include "../constants.h"
#include "unified_uniform_buffer.h"

#define def_uniform_buffer_data(_name, ...)                                                        \
  vulkan_##_name##_uniform_buffer_data *vulkan_##_name##_uniform_buffer_data_create(               \
      uint32_t count) {                                                                            \
    vulkan_##_name##_uniform_buffer_data *uniformBuffer =                                          \
        core_alloc(sizeof(vulkan_##_name##_uniform_buffer_data) +                                  \
                   count * sizeof(vulkan_##_name##_uniform_buffer_element));                       \
    uniformBuffer->bufferElement = (vulkan_buffer_element){0};                                     \
    uniformBuffer->count = count;                                                                  \
    for (size_t idx = 0; idx < count; idx++) {                                                     \
      uniformBuffer->elements[idx] = (vulkan_##_name##_uniform_buffer_element){0};                 \
    }                                                                                              \
    return uniformBuffer;                                                                          \
  }                                                                                                \
  void vulkan_##_name##_uniform_buffer_data_destroy(                                               \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer) {                                       \
    core_free(uniformBuffer);                                                                      \
  }                                                                                                \
  size_t vulkan_##_name##_uniform_buffer_data_get_size(                                            \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer) {                                       \
    return uniformBuffer->count * sizeof(vulkan_##_name##_uniform_buffer_element);                 \
  }                                                                                                \
  vulkan_##_name##_uniform_buffer_element *vulkan_##_name##_uniform_buffer_data_get_element(       \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t frame, size_t index) {           \
    assert(frame < MAX_FRAMES_IN_FLIGHT);                                                          \
    assert(frame *index < uniformBuffer->count);                                                   \
    return &uniformBuffer->elements[index];                                                        \
  }
VULKAN_UNIFORM_BUFFERS(def_uniform_buffer_data, )
#undef def_uniform_buffer_data

vulkan_descriptors *vulkan_descriptors_create(vulkan_device *vkd,
                                              vulkan_unified_uniform_buffer *unifiedUniformBuffer) {
  vulkan_descriptors *descriptors = core_alloc(sizeof(vulkan_descriptors));

  descriptors->vkd = vkd;
  descriptors->unifiedUniformBuffer = unifiedUniformBuffer;

  size_t totalUniformBufferDescriptorCount = 0;
#define binding_uniform_buffer(_name, ...) totalUniformBufferDescriptorCount++;
  VULKAN_UNIFORM_BUFFERS(binding_uniform_buffer, )
#undef binding_uniform_buffer
  size_t totalCombinedImageSamplerDescriptorCount = 1; // TODO: Textures.
  size_t maxAllocatedDescriptorSetsCount = 1;          // One large descriptor set
  descriptors->descriptorPool = vulkan_create_descriptor_pool(
      descriptors->vkd, totalUniformBufferDescriptorCount, totalCombinedImageSamplerDescriptorCount,
      maxAllocatedDescriptorSetsCount, "descriptors");

  size_t bindingNum = 0;
#define binding_uniform_buffer(_name, ...)                                                         \
  descriptors->_name##DescriptorSetNum = 0;                                                        \
  descriptors->_name##DescriptorBindingNum = bindingNum++;                                         \
  descriptors->_name##DescriptorCount = 1;
  VULKAN_UNIFORM_BUFFERS(binding_uniform_buffer, )
#undef binding_uniform_buffer
  assert(bindingNum > 0);

  VkDescriptorSetLayoutBinding layoutBindings[bindingNum];
  bindingNum = 0;
#define binding_uniform_buffer(_name, ...)                                                         \
  layoutBindings[bindingNum].binding = descriptors->_name##DescriptorBindingNum;                   \
  layoutBindings[bindingNum].descriptorCount = descriptors->_name##DescriptorCount;                \
  layoutBindings[bindingNum].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                   \
  layoutBindings[bindingNum].pImmutableSamplers = NULL;                                            \
  layoutBindings[bindingNum].stageFlags =                                                          \
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;                                   \
  bindingNum++;
  VULKAN_UNIFORM_BUFFERS(binding_uniform_buffer, )
#undef binding_uniform_buffer

  descriptors->descriptorSetLayout = vulkan_create_descriptor_set_layout(
      descriptors->vkd, layoutBindings, bindingNum, "descriptors");

  // HIRO create_descriptor_set_from_bindings
  descriptors->descriptorSet = vulkan_create_descriptor_set_for_uniform_buffers(
      descriptors->vkd, &descriptors->unifiedUniformBuffer->buffer->buffer,
      descriptors->unifiedUniformBuffer->buffer->totalSize, 1, descriptors->descriptorSetLayout,
      descriptors->descriptorPool, "descriptors");

  return descriptors;
}

void vulkan_descriptors_destroy(vulkan_descriptors *descriptors) {
  vkDestroyDescriptorSetLayout(descriptors->vkd->device, descriptors->descriptorSetLayout, vka);
  vkDestroyDescriptorPool(descriptors->vkd->device, descriptors->descriptorPool, vka);
}

void vulkan_scene_descriptors_update(vulkan_descriptors *descriptors) {
  // No-op.
}

void vulkan_descriptors_send_to_device(vulkan_descriptors *descriptors) {
  // No-op.
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
  log_debug(INDENT_FORMAT_STRING "descriptors:", INDENT_FORMAT_ARGS(0));
#define debug_print_uniform_buffer(_name, ...)                                                     \
  log_debug(INDENT_FORMAT_STRING "%s:\tset=%lu\tbinding=%lu\tcount=%lu", INDENT_FORMAT_ARGS(2),    \
            #_name, descriptors->_name##DescriptorSetNum,                                          \
            descriptors->_name##DescriptorBindingNum, descriptors->_name##DescriptorCount);
  VULKAN_UNIFORM_BUFFERS(debug_print_uniform_buffer, )
#undef debug_print_uniform_buffer
}

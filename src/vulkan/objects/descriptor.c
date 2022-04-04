#include "descriptor.h"

#include "../constants.h"
#include "textures.h"
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
    assert(frame < FRAMES_IN_FLIGHT);                                                              \
    assert(frame *index < uniformBuffer->count);                                                   \
    return &uniformBuffer->elements[index];                                                        \
  }
VULKAN_UNIFORM_BUFFERS(def_uniform_buffer_data, )
#undef def_uniform_buffer_data

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
  vulkan_buffer_element_debug_print(binding->bufferElement, 2);
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

  return descriptors;
}

void vulkan_descriptors_destroy(vulkan_descriptors *descriptors) {
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

void vulkan_descriptors_record_bind_command(vulkan_descriptors *descriptors,
                                            VkCommandBuffer commandBuffer,
                                            VkPipelineLayout pipelineLayout) {
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                          &descriptors->descriptorSet, 0, NULL);
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

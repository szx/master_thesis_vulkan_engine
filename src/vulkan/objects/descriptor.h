/* C structs used by shaders in uniform buffers. */

#pragma once

#include "buffer.h"

typedef struct vulkan_unified_uniform_buffer vulkan_unified_uniform_buffer;
typedef struct vulkan_textures vulkan_textures;

typedef struct vulkan_global_uniform_struct {
  mat4 viewMat;
  mat4 projMat;
} vulkan_global_uniform_struct;

typedef struct vulkan_instances_uniform_struct {
  mat4 modelMat;
} vulkan_instances_uniform_struct;

// *_uniform_element
// VULKAN_UNIFORM_BUFFERS
#include "../../codegen/descriptors.h"

// VULKAN_UNIFORM_BUFFER_COUNT
#define VULKAN_UNIFORM_BUFFER_COUNT_(_name, ...) +1
#define VULKAN_UNIFORM_BUFFER_COUNT (0 VULKAN_UNIFORM_BUFFERS(VULKAN_UNIFORM_BUFFER_COUNT_, ))

// *_uniform_buffer_data
// NOTE: Stores MAX_FRAMES_IN_FLIGHT*count elements, element n for frame #0, n+1 for frame #1 etc.
#define decl_uniform_buffer_data(_name, ...)                                                       \
  typedef struct vulkan_##_name##_uniform_buffer_data {                                            \
    vulkan_buffer_element bufferElement;                                                           \
    uint32_t count;                                                                                \
    vulkan_##_name##_uniform_buffer_element elements[];                                            \
  } vulkan_##_name##_uniform_buffer_data;                                                          \
  vulkan_##_name##_uniform_buffer_data *vulkan_##_name##_uniform_buffer_data_create(               \
      uint32_t count);                                                                             \
  void vulkan_##_name##_uniform_buffer_data_destroy(                                               \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer);                                        \
  size_t vulkan_##_name##_uniform_buffer_data_get_size(                                            \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer);                                        \
  vulkan_##_name##_uniform_buffer_element *vulkan_##_name##_uniform_buffer_data_get_element(       \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t frame, size_t index);
VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer_data, )
#undef decl_uniform_buffer_data

typedef struct vulkan_descriptors vulkan_descriptors;

/// Describes one descriptor binding in descriptor set.
typedef struct vulkan_descriptor_binding {
  vulkan_descriptors *descriptors;
  uint32_t bindingNumber;
  uint32_t descriptorCount;
  VkDescriptorType descriptorType;
  union {
    vulkan_buffer_element *bufferElement;
    vulkan_textures *textures;
  };
} vulkan_descriptor_binding;

void vulkan_descriptor_binding_debug_print(vulkan_descriptor_binding *binding, int indent);

/// Descriptors used to bind resources (uniform buffers and textures) to shaders.
/// Uniform buffers use classic slot-based binding model.
/// Textures use bindless model.
typedef struct vulkan_descriptors {
  vulkan_device *vkd;                                  ///< Pointer.
  vulkan_unified_uniform_buffer *unifiedUniformBuffer; ///< Pointer.
  vulkan_textures *textures;                           ///< Pointer.

  /* uniform buffers */
  /// Descriptor pool used to allocate descriptors for resources used by shaders.
  VkDescriptorPool descriptorPool;

  /// One giant descriptor set layout per one shader binding (layout qualifier).
  VkDescriptorSetLayout descriptorSetLayout;

  /// One giant descriptor set with one binding per resource.
  VkDescriptorSet descriptorSet;
  uint32_t descriptorSetNumber;

  vulkan_descriptor_binding uniformBufferBindings[VULKAN_UNIFORM_BUFFER_COUNT];
  vulkan_descriptor_binding texturesBinding;

  /// Pointers to named uniform buffer binding.
#define decl_uniform_buffer(_name, ...) vulkan_descriptor_binding *_name##UniformBufferBinding;
  VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer, )
#undef decl_uniform_buffer

} vulkan_descriptors;

vulkan_descriptors *vulkan_descriptors_create(vulkan_device *vkd,
                                              vulkan_unified_uniform_buffer *unifiedUniformBuffer,
                                              vulkan_textures *textures);

void vulkan_descriptors_destroy(vulkan_descriptors *descriptors);

void vulkan_descriptors_update(vulkan_descriptors *descriptors);

void vulkan_descriptors_send_to_device(vulkan_descriptors *descriptors);

void vulkan_descriptors_record_bind_command(vulkan_descriptors *descriptors,
                                            VkCommandBuffer commandBuffer,
                                            VkPipelineLayout pipelineLayout);
VkDescriptorSetLayout *
vulkan_descriptors_get_descriptor_set_layouts(vulkan_descriptors *descriptors, size_t *count);

void vulkan_descriptors_debug_print(vulkan_descriptors *descriptors, int indent);

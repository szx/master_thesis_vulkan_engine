/* C structs used by shaders in uniform buffers. */

#pragma once

#include "../../core/thirdparty.h"
#include "buffer.h"
#include "common.h"

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

// *_uniform_buffer_data
// NOTE: Stores MAX_FRAMES_IN_FLIGHT*count elements, element n for frame #0, n+1 for frame #1 etc.
#define decl_uniform_buffer_data(_name, ...)                                                       \
  typedef struct vulkan_##_name##_uniform_buffer_data {                                            \
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
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t frameInFlight, size_t index);
VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer_data, )
#undef decl_uniform_buffer_data

// HIRO get shader glsl source code
// HIRO get descriptor set

/* C structs used by shaders in uniform buffers. */

#pragma once

#include "../../core/thirdparty.h"
#include "buffer.h"
#include "common.h"

typedef struct vulkan_global_uniform_struct {
  mat4 viewMat;
  mat4 projMat;
} vulkan_global_uniform_struct;

typedef struct vulkan_instance_data_uniform_struct {
  mat4 modelMat;
} vulkan_instance_data_uniform_struct;

// *_uniform_element
// VULKAN_UNIFORM_BUFFERS
#include "../../codegen/descriptors.h"

// *_uniform_buffer_data
// NOTE: Stores MAX_FRAMES_IN_FLIGHT*count elements, element n for frame #0, n+1 for frame #1 etc.
#define decl_uniform_buffer_data(_name, ...)                                                       \
  typedef struct _name##_uniform_buffer_data {                                                     \
    uint32_t count;                                                                                \
    _name##_uniform_buffer_element elements[];                                                     \
  } _name##_uniform_buffer_data;                                                                   \
  _name##_uniform_buffer_data *_name##_uniform_buffer_data_create(uint32_t count);                 \
  void _name##_uniform_buffer_data_destroy(_name##_uniform_buffer_data *uniformBuffer);            \
  size_t _name##_uniform_buffer_data_get_size(_name##_uniform_buffer_data *uniformBuffer);         \
  _name##_uniform_buffer_element *_name##_uniform_buffer_data_get_element(                         \
      _name##_uniform_buffer_data *uniformBuffer, size_t frameInFlight, size_t index);
VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer_data, )
#undef decl_uniform_buffer_data

// HIRO get shader glsl source code
// HIRO get descriptor set

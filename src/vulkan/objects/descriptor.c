#include "descriptor.h"

#define def_uniform_buffer_data(_name, ...)                                                        \
  _name##_uniform_buffer_data *_name##_uniform_buffer_data_create(uint32_t count) {                \
    _name##_uniform_buffer_data *uniformBuffer = core_alloc(                                       \
        sizeof(_name##_uniform_buffer_data) + count * sizeof(_name##_uniform_buffer_element));     \
    uniformBuffer->count = count;                                                                  \
    for (size_t idx = 0; idx < count; idx++) {                                                     \
      uniformBuffer->elements[idx] = (_name##_uniform_buffer_element){0};                          \
    }                                                                                              \
    return uniformBuffer;                                                                          \
  }                                                                                                \
  void _name##_uniform_buffer_data_destroy(_name##_uniform_buffer_data *uniformBuffer) {           \
    core_free(uniformBuffer);                                                                      \
  }                                                                                                \
  size_t _name##_uniform_buffer_data_get_size(_name##_uniform_buffer_data *uniformBuffer) {        \
    return uniformBuffer->count * sizeof(_name##_uniform_buffer_element);                          \
  }
VULKAN_UNIFORM_BUFFERS(def_uniform_buffer_data, )
#undef def_uniform_buffer_data

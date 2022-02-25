#include "descriptor.h"
#include "../constants.h"

#define def_uniform_buffer_data(_name, ...)                                                        \
  vulkan_##_name##_uniform_buffer_data *vulkan_##_name##_uniform_buffer_data_create(               \
      uint32_t count) {                                                                            \
    vulkan_##_name##_uniform_buffer_data *uniformBuffer = core_alloc(                              \
        sizeof(vulkan_##_name##_uniform_buffer_data) +                                             \
        MAX_FRAMES_IN_FLIGHT * count * sizeof(vulkan_##_name##_uniform_buffer_element));           \
    uniformBuffer->count = count;                                                                  \
    for (size_t idx = 0; idx < MAX_FRAMES_IN_FLIGHT * count; idx++) {                              \
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
    return MAX_FRAMES_IN_FLIGHT * uniformBuffer->count *                                           \
           sizeof(vulkan_##_name##_uniform_buffer_element);                                        \
  }                                                                                                \
  vulkan_##_name##_uniform_buffer_element *vulkan_##_name##_uniform_buffer_data_get_element(       \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t frameInFlight, size_t index) {   \
    assert(frameInFlight < MAX_FRAMES_IN_FLIGHT);                                                  \
    assert(index < uniformBuffer->count);                                                          \
    return &uniformBuffer->elements[frameInFlight * index];                                        \
  }
VULKAN_UNIFORM_BUFFERS(def_uniform_buffer_data, )
#undef def_uniform_buffer_data

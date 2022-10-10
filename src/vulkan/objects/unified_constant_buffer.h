/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "buffer.h"
#include "shader.h"
#include "sync.h"

/// Structured buffer on top on global uniform buffer.
typedef struct unified_constant_buffer {
  /* CPU state */
#define x(_name, ...) _name##_uniform_buffer_data *_name##Data;
  VULKAN_UNIFORM_BUFFERS(x, )
#undef x

  /* GPU state */
  buffer *buffer;

} unified_constant_buffer;

unified_constant_buffer *unified_constant_buffer_create(device *vkd,
                                                        size_t maxPrimitiveElementCount);
void unified_constant_buffer_destroy(unified_constant_buffer *uniformBuffer);

typedef void (*unified_constant_buffer_update_func)(void *data, size_t currentFrameInFlight,
                                                    global_uniform_buffer_data *globalData,
                                                    instances_uniform_buffer_data *instancesData);

void unified_constant_buffer_update(
    unified_constant_buffer *uniformBuffer, sync *sync,
    unified_constant_buffer_update_func updateUnifiedConstantBufferFunc,
    void *updateUnifiedConstantBufferFuncData);

void unified_constant_buffer_send_to_device(unified_constant_buffer *uniformBuffer);

void unified_constant_buffer_debug_print(unified_constant_buffer *uniformBuffer);

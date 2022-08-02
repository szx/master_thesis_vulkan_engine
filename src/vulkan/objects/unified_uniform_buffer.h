/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "buffer.h"
#include "shader.h"
#include "sync.h"

typedef struct unified_uniform_buffer {
  /* CPU state */
  global_uniform_buffer_data *globalData;
  instances_uniform_buffer_data *instancesData;

  /* GPU state */
  buffer *buffer;

} unified_uniform_buffer;

unified_uniform_buffer *unified_uniform_buffer_create(device *vkd, size_t maxPrimitiveElementCount);
void unified_uniform_buffer_destroy(unified_uniform_buffer *uniformBuffer);

typedef void (*unified_uniform_buffer_update_func)(void *data, size_t currentFrameInFlight,
                                                   global_uniform_buffer_data *globalData,
                                                   instances_uniform_buffer_data *instancesData);

void unified_uniform_buffer_update(unified_uniform_buffer *uniformBuffer, sync *sync,
                                   unified_uniform_buffer_update_func updateGlobalUniformBufferFunc,
                                   void *updateGlobalUniformBufferFuncData);

void unified_uniform_buffer_send_to_device(unified_uniform_buffer *uniformBuffer);

void unified_uniform_buffer_debug_print(unified_uniform_buffer *uniformBuffer);

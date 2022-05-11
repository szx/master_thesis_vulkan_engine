/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "buffer.h"
#include "shader.h"
#include "sync.h"

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state */
  vulkan_global_uniform_buffer_data *globalData;
  vulkan_materials_uniform_buffer_data *materialsData;
  vulkan_instances_uniform_buffer_data *instancesData;

  /* GPU state */
  vulkan_buffer *buffer;

} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, size_t maxPrimitiveRenderCacheCount);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

typedef void (*vulkan_unified_uniform_buffer_update_func)(
    void *data, size_t currentFrameInFlight, vulkan_global_uniform_buffer_data *globalData,
    vulkan_materials_uniform_buffer_data *materialsData,
    vulkan_instances_uniform_buffer_data *instancesData);

void vulkan_unified_uniform_buffer_update(
    vulkan_unified_uniform_buffer *uniformBuffer, vulkan_sync *sync,
    vulkan_unified_uniform_buffer_update_func updateGlobalUniformBufferFunc,
    void *updateGlobalUniformBufferFuncData);

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);

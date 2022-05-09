/* Vulkan API helper objects. */

#pragma once

#include "../assets/object.h"
#include "buffer.h"
#include "camera.h"
#include "lights.h"
#include "render_cache_list.h"
#include "shader.h"
#include "skybox.h"
#include "sync.h"

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; /// Pointer.

  vulkan_global_uniform_buffer_data *globalData;
  vulkan_materials_uniform_buffer_data *materialsData;
  vulkan_instances_uniform_buffer_data *instancesData;

  /* GPU state */
  vulkan_buffer *buffer;

} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, vulkan_render_cache_list *renderCacheList);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer,
                                          vulkan_sync *sync, vulkan_camera *camera,
                                          vulkan_lights *lights, vulkan_skybox *skybox);

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);

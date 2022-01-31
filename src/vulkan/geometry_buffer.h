/* Basic Vulkan objects.
 * Used by scene to draw scene data. */

#pragma once

#include "camera.h"
#include "device.h"
#include "functions.h"

// HIRO rename to objects
// HIRO basic log-level wrappers: buffer
// HIRO basic high-level wrappers: vertex element, vertex buffer, uniform/storage buffer for shader

// Geometry buffer.
// Used to aggregate scene's vertex data into device local buffers.
typedef struct vulkan_geometry_buffer {
  /* initialized by vulkan_scene_build_geometry_buffer */
  UT_array *data; /// uint8_t
  /* initialized by vulkan_geometry_buffer_send_to_device */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;

  bool dirty; /// True if geometry buffer data updated on CPU.
} vulkan_geometry_buffer;

typedef struct vulkan_uniform_buffer {
  /* initialized by vulkan_uniform_buffer_create */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;
  VkDeviceSize bufferMemorySize;
  /* initialized and updated by vulkan_uniform_buffer_send_to_device */
  struct {
    alignas(16) mat4 viewMat;
    alignas(16) mat4 projMat;
  } data; // HIRO move UBO definitions out of vulkan_uniform_buffer

  bool dirty;            /// True if uniform buffer data updated on CPU.
} vulkan_uniform_buffer; // TODO: Manage every uniform buffers.

vulkan_geometry_buffer *vulkan_geometry_buffer_create();
void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer);
/// Creates device local buffer and copies geometry buffer data into it.
void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer);

/// Creates host visible and coherent buffer.
/// Descriptor set is created and managed by pipeline.
/// NOTE: We do not use device local memory, because we assume that mapping host visible and
///       coherent memory is faster than copying into device local memory using staging buffer.
vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd);
void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer);
/// Maps and updates uniform buffer memory.
void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer);

void vulkan_uniform_buffer_update_with_camera(vulkan_uniform_buffer *uniformBuffer,
                                              vulkan_camera *camera);

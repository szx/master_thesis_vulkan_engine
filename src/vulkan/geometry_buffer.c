#include "geometry_buffer.h"

vulkan_geometry_buffer *vulkan_geometry_buffer_create() {
  vulkan_geometry_buffer *geometryBuffer = core_alloc(sizeof(vulkan_geometry_buffer));
  static const UT_icd ut_vulkan_geometry_buffer_data_icd = {sizeof(uint8_t), NULL, NULL, NULL};
  utarray_new(geometryBuffer->data, &ut_vulkan_geometry_buffer_data_icd);
  geometryBuffer->vkd = NULL;
  geometryBuffer->buffer = VK_NULL_HANDLE;
  geometryBuffer->bufferMemory = VK_NULL_HANDLE;
  geometryBuffer->dirty = true;
  return geometryBuffer;
}

void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer) {
  utarray_free(geometryBuffer->data);
  if (geometryBuffer->vkd != NULL) {
    vkDestroyBuffer(geometryBuffer->vkd->device, geometryBuffer->buffer, vka);
    vkFreeMemory(geometryBuffer->vkd->device, geometryBuffer->bufferMemory, vka);
    geometryBuffer->vkd = NULL;
    geometryBuffer->buffer = VK_NULL_HANDLE;
    geometryBuffer->bufferMemory = VK_NULL_HANDLE;
  }
  core_free(geometryBuffer);
}

void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer) {
  if (!geometryBuffer->dirty) {
    return;
  }
  assert(geometryBuffer->buffer == VK_NULL_HANDLE);
  assert(geometryBuffer->bufferMemory == VK_NULL_HANDLE);
  // TODO: Reuse staging buffer.
  // TODO: Free geometry buffer data if geometry buffer is device local.
  geometryBuffer->vkd = vkd;
  size_t geometryBufferSize = utarray_len(geometryBuffer->data);
  uint8_t *geometryBufferData = utarray_front(geometryBuffer->data);
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  vulkan_create_buffer(geometryBuffer->vkd, geometryBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       &stagingBuffer, &stagingBufferMemory, "staging buffer for geometry");

  void *data;
  vkMapMemory(geometryBuffer->vkd->device, stagingBufferMemory, 0, geometryBufferSize, 0, &data);
  core_memcpy(data, geometryBufferData, geometryBufferSize);
  vkUnmapMemory(geometryBuffer->vkd->device, stagingBufferMemory);

  vulkan_create_buffer(geometryBuffer->vkd, geometryBufferSize,
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &geometryBuffer->buffer,
                       &geometryBuffer->bufferMemory, "geometry buffer");

  vulkan_copy_buffer_to_buffer(geometryBuffer->vkd, stagingBuffer, geometryBuffer->buffer,
                               geometryBufferSize);

  vkDestroyBuffer(geometryBuffer->vkd->device, stagingBuffer, vka);
  vkFreeMemory(geometryBuffer->vkd->device, stagingBufferMemory, vka);
  geometryBuffer->dirty = false;
}

vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd) {
  vulkan_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_uniform_buffer));
  glm_mat4_identity(uniformBuffer->data.viewMat);
  glm_mat4_identity(uniformBuffer->data.projMat);
  uniformBuffer->vkd = vkd;
  uniformBuffer->bufferMemorySize = sizeof(uniformBuffer->data);
  vulkan_create_buffer(uniformBuffer->vkd, uniformBuffer->bufferMemorySize,
                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       &uniformBuffer->buffer, &uniformBuffer->bufferMemory, "uniform buffer");
  uniformBuffer->dirty = true;
  return uniformBuffer;
  // TODO support multiple buffers and descriptors (seperate for each frame)
}

void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer) {
  assert(uniformBuffer->vkd != NULL);
  vkDestroyBuffer(uniformBuffer->vkd->device, uniformBuffer->buffer, vka);
  vkFreeMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory, vka);
  uniformBuffer->vkd = NULL;
  uniformBuffer->buffer = VK_NULL_HANDLE;
  uniformBuffer->bufferMemory = VK_NULL_HANDLE;
  uniformBuffer->bufferMemorySize = 0;
  core_free(uniformBuffer);
}

void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer) {
  assert(uniformBuffer->buffer != VK_NULL_HANDLE);
  assert(uniformBuffer->bufferMemory != VK_NULL_HANDLE);
  if (!uniformBuffer->dirty) {
    return;
  }
  void *data;
  vkMapMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory, 0,
              uniformBuffer->bufferMemorySize, 0, &data);
  core_memcpy(data, &uniformBuffer->data, uniformBuffer->bufferMemorySize);
  vkUnmapMemory(uniformBuffer->vkd->device, uniformBuffer->bufferMemory);
  uniformBuffer->dirty = false;
}

void vulkan_uniform_buffer_update_with_camera(vulkan_uniform_buffer *uniformBuffer,
                                              vulkan_camera_data *camera) {
  if (!camera->dirty) {
    return;
  }
  vec3 negativePosition;
  mat4 translationMat;
  glm_vec3_negate_to(camera->position, negativePosition);
  glm_translate_make(translationMat, negativePosition);
  mat4 rotationMat;
  glm_quat_mat4(camera->rotation, rotationMat);
  glm_mat4_mul(rotationMat, translationMat, uniformBuffer->data.viewMat);

  glm_perspective(camera->fovY, camera->aspectRatio, camera->nearZ, camera->farZ,
                  uniformBuffer->data.projMat);
  uniformBuffer->data.projMat[1][1] *= -1; // invert for Y+ pointing up
  camera->dirty = false;
  uniformBuffer->dirty = true;
}

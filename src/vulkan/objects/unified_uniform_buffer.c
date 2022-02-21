#include "unified_uniform_buffer.h"

vulkan_unified_uniform_buffer *vulkan_unified_uniform_buffer_create(vulkan_device *vkd) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));
  // HIRO: Determine number of instances from cache list (accumulate in it?).
  uniformBuffer->instanceData = vulkan_instance_data_uniform_buffer_data_create(100);
  for (size_t idx = 0; idx < uniformBuffer->instanceData->count; idx++) {
    glm_mat4_identity(uniformBuffer->instanceData->elements[idx].viewMat);
    glm_mat4_identity(uniformBuffer->instanceData->elements[idx].projMat);
  }
  // HIRO how to handle multiple _uniform_buffer_data? (alignment)
  size_t size = vulkan_instance_data_uniform_buffer_data_get_size(uniformBuffer->instanceData);
  uniformBuffer->buffer =
      vulkan_buffer_create(vkd, vulkan_buffer_type_uniform, uniformBuffer->instanceData, size);

  uniformBuffer->dirty = true;
  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_instance_data_uniform_buffer_data_destroy(uniformBuffer->instanceData);
  core_free(uniformBuffer);
}

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer) {
  uniformBuffer->buffer->dirty = uniformBuffer->dirty;
  vulkan_buffer_send_to_device(uniformBuffer->buffer);
  uniformBuffer->dirty = false;
}

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer) {
  log_debug("UNIFIED UNIFORM BUFFER:\n");
  assert(uniformBuffer->buffer->size > 0);
  log_debug("uniform buffer size=%d\n", uniformBuffer->buffer->size);
  log_debug("instance data count=%d\n", uniformBuffer->instanceData->count);
}

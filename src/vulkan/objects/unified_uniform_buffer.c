#include "unified_uniform_buffer.h"

vulkan_unified_uniform_buffer *vulkan_unified_uniform_buffer_create(vulkan_device *vkd) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));

  glm_mat4_identity(uniformBuffer->data.viewMat);
  glm_mat4_identity(uniformBuffer->data.projMat);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform,
                                               &uniformBuffer->data, sizeof(uniformBuffer->data));

  uniformBuffer->dirty = true;
  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_buffer_destroy(uniformBuffer->buffer);
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
  log_debug("buffer size=%d\n", uniformBuffer->buffer->size);
}

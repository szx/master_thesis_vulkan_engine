#include "unified_uniform_buffer.h"
#include "../assets/primitive.h"
#include "textures.h"

unified_uniform_buffer *unified_uniform_buffer_create(device *vkd,
                                                      size_t maxPrimitiveElementCount) {
  unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(unified_uniform_buffer));

  uniformBuffer->globalData = global_uniform_buffer_data_create(1, FRAMES_IN_FLIGHT);
  uniformBuffer->instancesData =
      instances_uniform_buffer_data_create(maxPrimitiveElementCount, FRAMES_IN_FLIGHT);

  uniformBuffer->buffer = buffer_create(vkd, buffer_type_uniform);

  uniformBuffer->globalData->bufferElement =
      buffer_add(uniformBuffer->buffer, &uniformBuffer->globalData->elements,
                 global_uniform_buffer_data_get_size(uniformBuffer->globalData));
  uniformBuffer->instancesData->bufferElement =
      buffer_add(uniformBuffer->buffer, &uniformBuffer->instancesData->elements,
                 instances_uniform_buffer_data_get_size(uniformBuffer->instancesData));

  buffer_make_resident(uniformBuffer->buffer);

  return uniformBuffer;
}

void unified_uniform_buffer_destroy(unified_uniform_buffer *uniformBuffer) {
  instances_uniform_buffer_data_destroy(uniformBuffer->instancesData);
  global_uniform_buffer_data_destroy(uniformBuffer->globalData);
  buffer_destroy(uniformBuffer->buffer);
  core_free(uniformBuffer);
}

void unified_uniform_buffer_update(unified_uniform_buffer *uniformBuffer, sync *sync,
                                   unified_uniform_buffer_update_func updateGlobalUniformBufferFunc,
                                   void *updateGlobalUniformBufferFuncData) {

  updateGlobalUniformBufferFunc(updateGlobalUniformBufferFuncData, sync->currentFrameInFlight,
                                uniformBuffer->globalData, uniformBuffer->instancesData);

  // TODO: Dirty only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;
}

void unified_uniform_buffer_send_to_device(unified_uniform_buffer *uniformBuffer) {
  // TODO: Update only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;
  buffer_send_to_device(uniformBuffer->buffer);
}

void unified_uniform_buffer_debug_print(unified_uniform_buffer *uniformBuffer) {
  log_debug("UNIFIED UNIFORM BUFFER:\n");
  assert(uniformBuffer->buffer->totalSize > 0);
  log_debug("uniform buffer size=%d\n", uniformBuffer->buffer->totalSize);
  log_debug("global data count=%d\n",
            global_uniform_buffer_data_get_count(uniformBuffer->globalData));
  log_debug("instances data count=%d\n",
            instances_uniform_buffer_data_get_count(uniformBuffer->instancesData));
}

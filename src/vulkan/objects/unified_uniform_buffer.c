#include "unified_uniform_buffer.h"
#include "../assets/primitive.h"
#include "textures.h"

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, size_t maxPrimitiveRenderCacheCount) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));

  uniformBuffer->globalData = vulkan_global_uniform_buffer_data_create(1, FRAMES_IN_FLIGHT);
  uniformBuffer->materialsData =
      vulkan_materials_uniform_buffer_data_create(MAX_MATERIAL_COUNT, FRAMES_IN_FLIGHT);
  uniformBuffer->instancesData =
      vulkan_instances_uniform_buffer_data_create(maxPrimitiveRenderCacheCount, FRAMES_IN_FLIGHT);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform);

  uniformBuffer->globalData->bufferElement =
      vulkan_buffer_add(uniformBuffer->buffer, &uniformBuffer->globalData->elements,
                        vulkan_global_uniform_buffer_data_get_size(uniformBuffer->globalData));
  uniformBuffer->materialsData->bufferElement = vulkan_buffer_add(
      uniformBuffer->buffer, &uniformBuffer->materialsData->elements,
      vulkan_materials_uniform_buffer_data_get_size(uniformBuffer->materialsData));
  uniformBuffer->instancesData->bufferElement = vulkan_buffer_add(
      uniformBuffer->buffer, &uniformBuffer->instancesData->elements,
      vulkan_instances_uniform_buffer_data_get_size(uniformBuffer->instancesData));

  vulkan_buffer_make_resident(uniformBuffer->buffer);

  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_instances_uniform_buffer_data_destroy(uniformBuffer->instancesData);
  vulkan_materials_uniform_buffer_data_destroy(uniformBuffer->materialsData);
  vulkan_global_uniform_buffer_data_destroy(uniformBuffer->globalData);
  vulkan_buffer_destroy(uniformBuffer->buffer);
  core_free(uniformBuffer);
}

void vulkan_unified_uniform_buffer_update(
    vulkan_unified_uniform_buffer *uniformBuffer, vulkan_sync *sync,
    vulkan_unified_uniform_buffer_update_func updateGlobalUniformBufferFunc,
    void *updateGlobalUniformBufferFuncData) {

  updateGlobalUniformBufferFunc(updateGlobalUniformBufferFuncData, sync->currentFrameInFlight,
                                uniformBuffer->globalData, uniformBuffer->materialsData,
                                uniformBuffer->instancesData);

  // TODO: Dirty only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;
}

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer) {
  // TODO: Update only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;
  vulkan_buffer_send_to_device(uniformBuffer->buffer);
}

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer) {
  log_debug("UNIFIED UNIFORM BUFFER:\n");
  assert(uniformBuffer->buffer->totalSize > 0);
  log_debug("uniform buffer size=%d\n", uniformBuffer->buffer->totalSize);
  log_debug("global data count=%d\n",
            vulkan_global_uniform_buffer_data_get_count(uniformBuffer->globalData));
  log_debug("materials data count=%d\n",
            vulkan_materials_uniform_buffer_data_get_count(uniformBuffer->materialsData));
  log_debug("instances data count=%d\n",
            vulkan_instances_uniform_buffer_data_get_count(uniformBuffer->instancesData));
}

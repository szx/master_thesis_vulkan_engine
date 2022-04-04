#include "unified_uniform_buffer.h"
#include "../data/camera.h"

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd,
                                     vulkan_render_cache_list *renderCacheList) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));
  uniformBuffer->renderCacheList = renderCacheList;

  uniformBuffer->instancesData =
      vulkan_instances_uniform_buffer_data_create(renderCacheList->maxPrimitiveRenderCacheCount);
  uniformBuffer->globalData = vulkan_global_uniform_buffer_data_create(1);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform);

  uniformBuffer->instancesData->bufferElement = vulkan_buffer_add(
      uniformBuffer->buffer, &uniformBuffer->instancesData->elements,
      vulkan_instances_uniform_buffer_data_get_size(uniformBuffer->instancesData));
  uniformBuffer->globalData->bufferElement =
      vulkan_buffer_add(uniformBuffer->buffer, &uniformBuffer->globalData->elements,
                        vulkan_global_uniform_buffer_data_get_size(uniformBuffer->globalData));

  vulkan_buffer_make_resident(uniformBuffer->buffer);

  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_global_uniform_buffer_data_destroy(uniformBuffer->globalData);
  vulkan_instances_uniform_buffer_data_destroy(uniformBuffer->instancesData);
  vulkan_buffer_destroy(uniformBuffer->buffer);
  core_free(uniformBuffer);
}

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer,
                                          vulkan_camera *camera) {
  assert(utarray_len(uniformBuffer->renderCacheList->primitiveRenderCaches) > 0);
  assert(utarray_len(uniformBuffer->renderCacheList->cameraRenderCaches) > 0);

  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              uniformBuffer->renderCacheList->primitiveRenderCaches) {
    size_t instanceId = renderCache->renderCacheListIdx;
    vulkan_instances_uniform_buffer_element *element =
        &uniformBuffer->instancesData->elements[instanceId];
    // HIRO update only for frame in flight. (size_t frameInFlight argument)
    glm_mat4_copy(renderCache->transform, element->modelMat);
    size_t materialId = renderCache->renderCacheListIdx;
    element->materialId = materialId; // HIRO HIRO material id ubo.
    // HIRO HIRO material id in render cache
  }

  if (true /*cameraObject->camera->dirty*/) {
    vulkan_global_uniform_buffer_element *element = &uniformBuffer->globalData->elements[0];
    vulkan_camera_set_view_matrix(camera, element->viewMat);
    vulkan_camera_set_projection_matrix(camera, element->projMat);
  }

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
  log_debug("instance data count=%d\n", uniformBuffer->instancesData->count);
  log_debug("global data count=%d\n", uniformBuffer->globalData->count);
}

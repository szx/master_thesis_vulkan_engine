#include "unified_uniform_buffer.h"
#include "../data/camera.h"

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd,
                                     vulkan_render_cache_list *renderCacheList) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));
  uniformBuffer->renderCacheList = renderCacheList;

  uniformBuffer->instancesData =
      vulkan_instances_uniform_buffer_data_create(renderCacheList->maxCount);
  uniformBuffer->globalData = vulkan_global_uniform_buffer_data_create(1);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform);

  uniformBuffer->instancesData->bufferElement = vulkan_buffer_add(
      uniformBuffer->buffer, &uniformBuffer->instancesData->elements,
      vulkan_instances_uniform_buffer_data_get_size(uniformBuffer->instancesData));
  uniformBuffer->globalData->bufferElement =
      vulkan_buffer_add(uniformBuffer->buffer, &uniformBuffer->globalData->elements,
                        vulkan_global_uniform_buffer_data_get_size(uniformBuffer->globalData));

  vulkan_buffer_make_resident(uniformBuffer->buffer);

  uniformBuffer->descriptors = vulkan_descriptors_create(vkd, uniformBuffer);

  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_global_uniform_buffer_data_destroy(uniformBuffer->globalData);
  vulkan_instances_uniform_buffer_data_destroy(uniformBuffer->instancesData);
  vulkan_descriptors_destroy(uniformBuffer->descriptors);
  vulkan_buffer_destroy(uniformBuffer->buffer);
  core_free(uniformBuffer);
}

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer,
                                          vulkan_data_camera *camera) {
  assert(utarray_len(uniformBuffer->renderCacheList->caches) > 0);

  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              uniformBuffer->renderCacheList->caches) {
    size_t instanceId = renderCache->renderCacheListIdx;
    // HIRO update only for frame in flight. (size_t frameInFlight argument)
    glm_mat4_copy(renderCache->transform,
                  uniformBuffer->instancesData->elements[instanceId].modelMat);
  }

  if (camera->dirty) {
    vec3 negativePosition;
    mat4 translationMat;
    glm_vec3_negate_to(camera->position, negativePosition);
    glm_translate_make(translationMat, negativePosition);
    mat4 rotationMat;
    glm_quat_mat4(camera->rotation, rotationMat);

    vulkan_global_uniform_buffer_element *element = &uniformBuffer->globalData->elements[0];
    glm_mat4_mul(rotationMat, translationMat, element->viewMat);
    glm_perspective(camera->fovY, camera->aspectRatio, camera->nearZ, camera->farZ,
                    element->projMat);
    element->projMat[1][1] *= -1; // invert for Y+ pointing up

    camera->dirty = false;
  }

  // TODO: Dirty only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;

  vulkan_scene_descriptors_update(uniformBuffer->descriptors);
}

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer) {
  // TODO: Update only parts of unified uniform buffer.
  uniformBuffer->buffer->dirty = true;
  vulkan_buffer_send_to_device(uniformBuffer->buffer);

  vulkan_descriptors_send_to_device(uniformBuffer->descriptors);
}

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer) {
  log_debug("UNIFIED UNIFORM BUFFER:\n");
  assert(uniformBuffer->buffer->totalSize > 0);
  log_debug("uniform buffer size=%d\n", uniformBuffer->buffer->totalSize);
  log_debug("instance data count=%d\n", uniformBuffer->instancesData->count);
  log_debug("global data count=%d\n", uniformBuffer->globalData->count);
  vulkan_descriptors_debug_print(uniformBuffer->descriptors, 2);
}

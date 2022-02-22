#include "unified_uniform_buffer.h"
#include "../data/camera.h"

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, vulkan_render_cache_list *renderCacheList,
                                     vulkan_data_camera *camera) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));
  uniformBuffer->renderCacheList = renderCacheList;
  uniformBuffer->camera = camera;

  uniformBuffer->instanceData =
      vulkan_instance_data_uniform_buffer_data_create(renderCacheList->maxCount);
  uniformBuffer->globalData = vulkan_global_uniform_buffer_data_create(1);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform);
  vulkan_buffer_add(uniformBuffer->buffer, &uniformBuffer->instanceData->elements,
                    vulkan_instance_data_uniform_buffer_data_get_size(uniformBuffer->instanceData));
  vulkan_buffer_add(uniformBuffer->buffer, &uniformBuffer->globalData->elements,
                    vulkan_global_uniform_buffer_data_get_size(uniformBuffer->globalData));

  uniformBuffer->dirty = true;

  return uniformBuffer;
}

void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer) {
  vulkan_global_uniform_buffer_data_destroy(uniformBuffer->globalData);
  vulkan_instance_data_uniform_buffer_data_destroy(uniformBuffer->instanceData);
  vulkan_unified_uniform_buffer_destroy(uniformBuffer);
  core_free(uniformBuffer);
}

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer) {
  assert(utarray_len(uniformBuffer->renderCacheList->caches) > 0);

  for (size_t idx = 0; idx < uniformBuffer->instanceData->count; idx++) {
    vulkan_render_cache *cache = utarray_eltptr(uniformBuffer->renderCacheList->caches, idx);
    glm_mat4_copy(cache->transform, uniformBuffer->instanceData->elements[idx].modelMat);
  }

  if (uniformBuffer->camera->dirty) {
    vulkan_data_camera *camera = uniformBuffer->camera;

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

  uniformBuffer->dirty = true;
}

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer) {
  uniformBuffer->buffer->dirty = uniformBuffer->dirty;
  vulkan_buffer_send_to_device(uniformBuffer->buffer);
  uniformBuffer->dirty = false;
}

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer) {
  log_debug("UNIFIED UNIFORM BUFFER:\n");
  assert(uniformBuffer->buffer->totalSize > 0);
  log_debug("uniform buffer size=%d\n", uniformBuffer->buffer->totalSize);
  log_debug("instance data count=%d\n", uniformBuffer->instanceData->count);
  log_debug("global data count=%d\n", uniformBuffer->globalData->count);
}

#include "unified_uniform_buffer.h"
#include "../data/camera.h"
#include "../data/primitive.h"
#include "textures.h"

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd,
                                     vulkan_render_cache_list *renderCacheList) {
  vulkan_unified_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_unified_uniform_buffer));
  uniformBuffer->renderCacheList = renderCacheList;

  uniformBuffer->globalData = vulkan_global_uniform_buffer_data_create(1);
  uniformBuffer->materialsData = vulkan_materials_uniform_buffer_data_create(MAX_MATERIAL_COUNT);
  uniformBuffer->instancesData = vulkan_instances_uniform_buffer_data_create(
      renderCacheList->maxPrimitiveRenderCacheCount * FRAMES_IN_FLIGHT);

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

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer,
                                          vulkan_sync *sync, vulkan_camera *camera) {
  assert(utarray_len(uniformBuffer->renderCacheList->primitiveRenderCaches) > 0);
  assert(utarray_len(uniformBuffer->renderCacheList->cameraRenderCaches) > 0);

  // global
  if (true /*cameraObject->camera->dirty*/) {
    vulkan_global_uniform_buffer_element *element = &uniformBuffer->globalData->elements[0];
    vulkan_camera_set_view_matrix(camera, element->viewMat);
    vulkan_camera_set_projection_matrix(camera, element->projMat);
    element->lights[0].position[0] = 1.0f; // HIRO HIRO set lights
  }

  // materials
  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              uniformBuffer->renderCacheList->primitiveRenderCaches) {
    vulkan_textures_material_element *materialElement = renderCache->materialElement;
    assert(materialElement != NULL);
    size_t materialId = materialElement->materialIdx;
    // PERF: Update material only once (either keep track here or just iterate on
    // textures->materialElements).
    log_debug("updating material %zu", materialId);
    vulkan_materials_uniform_buffer_element *element =
        &uniformBuffer->materialsData->elements[materialId];

    element->baseColorTextureId = materialElement->baseColorTextureElement->textureIdx;
    glm_vec4_copy(materialElement->material->baseColorFactor, element->baseColorFactor);
    element->metallicRoughnessTextureId =
        materialElement->metallicRoughnessTextureElement->textureIdx;
    element->metallicFactor = materialElement->material->metallicFactor;
    element->roughnessFactor = materialElement->material->roughnessFactor;
  }

  // instances
  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              uniformBuffer->renderCacheList->primitiveRenderCaches) {
    size_t instanceId = FRAMES_IN_FLIGHT * renderCache->instanceId + sync->currentFrameInFlight;
    vulkan_instances_uniform_buffer_element *element =
        &uniformBuffer->instancesData->elements[instanceId];

    glm_mat4_copy(renderCache->transform, element->modelMat);

    size_t materialId = renderCache->materialElement->materialIdx;
    element->materialId = materialId;
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
  log_debug("global data count=%d\n", uniformBuffer->globalData->count);
  log_debug("materials data count=%d\n", uniformBuffer->materialsData->count);
  log_debug("instances data count=%d\n", uniformBuffer->instancesData->count);
}

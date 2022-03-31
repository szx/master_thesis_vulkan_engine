#include "render_state.h"

vulkan_render_state *vulkan_render_state_create(vulkan_device *vkd,
                                                vulkan_render_cache_list *renderCacheList) {
  assert(utarray_len(renderCacheList->primitiveRenderCaches) > 0);
  assert(utarray_len(renderCacheList->cameraRenderCaches) > 0);

  vulkan_render_state *renderState = core_alloc(sizeof(vulkan_render_state));

  renderState->renderCacheList = renderCacheList;
  renderState->camera = vulkan_camera_create(renderState->renderCacheList);

  renderState->batches = vulkan_batches_create(renderCacheList);

  renderState->vkd = vkd;
  renderState->unifiedGeometryBuffer =
      vulkan_unified_geometry_buffer_create(renderState->vkd, renderState->renderCacheList);
  renderState->unifiedUniformBuffer =
      vulkan_unified_uniform_buffer_create(vkd, renderState->renderCacheList);

  renderState->descriptors = vulkan_descriptors_create(vkd, renderState->unifiedUniformBuffer);

  renderState->sync = vulkan_sync_create(renderState->vkd);

  vulkan_scene_render_state_update(renderState);

  return renderState;
}

void vulkan_render_state_destroy(vulkan_render_state *renderState) {
  vulkan_sync_destroy(renderState->sync);

  vulkan_descriptors_destroy(renderState->descriptors);

  vulkan_unified_uniform_buffer_destroy(renderState->unifiedUniformBuffer);
  vulkan_unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);

  vulkan_batches_destroy(renderState->batches);

  vulkan_camera_destroy(renderState->camera);

  core_free(renderState);
}

void vulkan_scene_render_state_update(vulkan_render_state *renderState) {

  vulkan_batch_policy batchPolicy = vulkan_batch_policy_matching_materials;
  vulkan_batches_update(renderState->batches, batchPolicy);
  dl_foreach_elem(vulkan_batch *, batch, renderState->batches->batches, {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  })

  vulkan_unified_geometry_buffer_update(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_update(renderState->unifiedUniformBuffer, renderState->camera);

  vulkan_descriptors_update(renderState->descriptors);
}

void vulkan_render_state_send_to_device(vulkan_render_state *renderState) {
  vulkan_unified_geometry_buffer_send_to_device(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_send_to_device(renderState->unifiedUniformBuffer);

  vulkan_descriptors_send_to_device(renderState->descriptors);

  assert(renderState->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderState->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderState->unifiedUniformBuffer->buffer->resident);
}

void vulkan_render_state_debug_print(vulkan_render_state *renderState) {
  log_debug("RENDER STATE:\n");
  vulkan_batches_debug_print(renderState->batches);
  vulkan_unified_geometry_buffer_debug_print(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderState->unifiedUniformBuffer);
  vulkan_descriptors_debug_print(renderState->descriptors, 2);
  vulkan_sync_debug_print(renderState->sync, 2);
}

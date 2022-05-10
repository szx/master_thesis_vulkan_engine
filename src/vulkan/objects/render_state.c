#include "render_state.h"

vulkan_render_state *vulkan_render_state_create(
    vulkan_swap_chain *vks, vulkan_render_cache_list *renderCacheList, data_config *config,
    vulkan_unified_uniform_buffer_update_func updateGlobalUniformBufferFunc) {
  assert(utarray_len(renderCacheList->primitiveRenderCaches) > 0);

  vulkan_render_state *renderState = core_alloc(sizeof(vulkan_render_state));

  renderState->vks = vks;
  renderState->vkd = renderState->vks->vkd;

  renderState->renderCacheList = renderCacheList;
  renderState->config = config;
  renderState->updateGlobalUniformBufferFunc = updateGlobalUniformBufferFunc;
  renderState->vertexStream = vulkan_vertex_stream_create(renderState->renderCacheList);

  renderState->unifiedGeometryBuffer = vulkan_unified_geometry_buffer_create(renderState->vkd);
  renderState->textures = vulkan_textures_create(renderState->vkd);
  renderState->unifiedUniformBuffer = vulkan_unified_uniform_buffer_create(
      renderState->vkd, renderState->renderCacheList->maxPrimitiveRenderCacheCount);
  renderState->descriptors = vulkan_descriptors_create(
      renderState->vkd, renderState->unifiedUniformBuffer, renderState->textures);
  renderState->sync = vulkan_sync_create(renderState->vkd);

  // HIRO refactor is it needed? vulkan_render_state_update(renderState);

  return renderState;
}

void vulkan_render_state_destroy(vulkan_render_state *renderState) {
  vulkan_sync_destroy(renderState->sync);

  vulkan_descriptors_destroy(renderState->descriptors);
  vulkan_unified_uniform_buffer_destroy(renderState->unifiedUniformBuffer);
  vulkan_textures_destroy(renderState->textures);
  vulkan_unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);

  vulkan_vertex_stream_destroy(renderState->vertexStream);

  core_free(renderState);
}

void vulkan_render_state_update(vulkan_render_state *renderState,
                                void *updateGlobalUniformBufferFuncData) {
  vulkan_render_cache_list_update_textures(renderState->renderCacheList, renderState->textures);
  vulkan_vertex_stream_update(renderState->vertexStream);

  vulkan_unified_geometry_buffer_update(renderState->unifiedGeometryBuffer,
                                        renderState->vertexStream);
  vulkan_textures_update(renderState->textures);
  vulkan_unified_uniform_buffer_update(
      renderState->unifiedUniformBuffer, renderState->renderCacheList, renderState->sync,
      renderState->updateGlobalUniformBufferFunc, updateGlobalUniformBufferFuncData);
  vulkan_descriptors_update(renderState->descriptors);
}

void vulkan_render_state_send_to_device(vulkan_render_state *renderState) {
  vulkan_unified_geometry_buffer_send_to_device(renderState->unifiedGeometryBuffer);
  vulkan_textures_send_to_device(renderState->textures);
  vulkan_unified_uniform_buffer_send_to_device(renderState->unifiedUniformBuffer);
  vulkan_descriptors_send_to_device(renderState->descriptors);

  assert(renderState->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderState->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderState->unifiedUniformBuffer->buffer->resident);
}

void vulkan_render_state_debug_print(vulkan_render_state *renderState) {
  log_debug("render state:\n");
  vulkan_unified_geometry_buffer_debug_print(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderState->unifiedUniformBuffer);
  vulkan_textures_debug_print(renderState->textures, 2);
  vulkan_descriptors_debug_print(renderState->descriptors, 2);
  vulkan_sync_debug_print(renderState->sync, 2);
}

#include "render_state.h"

render_state *
render_state_create(swap_chain *vks, renderer_cache *rendererCache, data_config *config,
                    unified_uniform_buffer_update_func updateGlobalUniformBufferFunc) {

  assert(dl_count(rendererCache->primitiveElements) > 0);

  render_state *renderState = core_alloc(sizeof(render_state));

  renderState->vks = vks;
  renderState->vkd = renderState->vks->vkd;

  renderState->rendererCache = rendererCache;
  renderState->config = config;
  renderState->updateGlobalUniformBufferFunc = updateGlobalUniformBufferFunc;
  renderState->vertexStream = vertex_stream_create(renderState->rendererCache->attributes);

  renderState->unifiedGeometryBuffer = unified_geometry_buffer_create(renderState->vkd);
  renderState->textures = textures_create(renderState->vkd);
  renderState->unifiedUniformBuffer = unified_uniform_buffer_create(
      renderState->vkd, renderState->rendererCache->maxPrimitiveElementCount);
  renderState->descriptors = descriptors_create(renderState->vkd, renderState->unifiedUniformBuffer,
                                                renderState->textures);
  renderState->sync = sync_create(renderState->vkd);

  return renderState;
}

void render_state_destroy(render_state *renderState) {
  sync_destroy(renderState->sync);

  descriptors_destroy(renderState->descriptors);
  unified_uniform_buffer_destroy(renderState->unifiedUniformBuffer);
  textures_destroy(renderState->textures);
  unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);

  vertex_stream_destroy(renderState->vertexStream);

  core_free(renderState);
}

void render_state_update(render_state *renderState, void *updateGlobalUniformBufferFuncData) {
  renderer_cache_update_geometry(renderState->rendererCache, renderState->vertexStream);
  renderer_cache_update_textures(renderState->rendererCache, renderState->textures);
  vertex_stream_update(renderState->vertexStream);

  unified_geometry_buffer_update(renderState->unifiedGeometryBuffer, renderState->vertexStream);
  textures_update(renderState->textures);
  unified_uniform_buffer_update(renderState->unifiedUniformBuffer, renderState->sync,
                                renderState->updateGlobalUniformBufferFunc,
                                updateGlobalUniformBufferFuncData);
  descriptors_update(renderState->descriptors);
}

void render_state_send_to_device(render_state *renderState) {
  unified_geometry_buffer_send_to_device(renderState->unifiedGeometryBuffer);
  textures_send_to_device(renderState->textures);
  unified_uniform_buffer_send_to_device(renderState->unifiedUniformBuffer);
  descriptors_send_to_device(renderState->descriptors);

  assert(renderState->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderState->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderState->unifiedUniformBuffer->buffer->resident);
}

void render_state_debug_print(render_state *renderState) {
  log_debug("render state:\n");
  unified_geometry_buffer_debug_print(renderState->unifiedGeometryBuffer);
  unified_uniform_buffer_debug_print(renderState->unifiedUniformBuffer);
  textures_debug_print(renderState->textures, 2);
  descriptors_debug_print(renderState->descriptors, 2);
  sync_debug_print(renderState->sync, 2);
}

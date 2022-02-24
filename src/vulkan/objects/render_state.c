#include "render_state.h"

vulkan_render_state *vulkan_render_state_create(vulkan_device *vkd,
                                                vulkan_render_cache_list *renderCacheList,
                                                vulkan_data_camera *camera) {
  assert(renderCacheList->attributes > 0);
  assert(utarray_len(renderCacheList->caches) > 0);

  vulkan_render_state *renderState = core_alloc(sizeof(vulkan_render_state));

  renderState->batches = vulkan_batches_create(renderCacheList);
  renderState->camera = camera;
  vulkan_interleaved_vertex_stream *stream =
      vulkan_interleaved_vertex_stream_create(renderCacheList);

  renderState->unifiedGeometryBuffer = vulkan_unified_geometry_buffer_create(vkd, stream);
  renderState->unifiedUniformBuffer = vulkan_unified_uniform_buffer_create(vkd, renderCacheList);

  vulkan_scene_render_state_update(renderState);

  return renderState;
}

void vulkan_scene_render_state_update(vulkan_render_state *renderState) {

  /* update unified geometry buffer */
  vulkan_interleaved_vertex_stream_update(renderState->interleavedVertexStream, false);

  /* update batches */
  vulkan_batch_policy batchPolicy = vulkan_batch_policy_matching_materials;
  vulkan_batches_update(renderState->batches, batchPolicy);
  dl_foreach_elem(vulkan_batch *, batch, renderState->batches->batches, {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  })

  /* update unified uniform buffer */
  vulkan_unified_uniform_buffer_update(renderState->unifiedUniformBuffer, renderState->camera);
}

void vulkan_render_state_destroy(vulkan_render_state *renderState) {
  vulkan_batches_destroy(renderState->batches);
  vulkan_unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);
  vulkan_unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);
  core_free(renderState);
}

void vulkan_render_state_send_to_device(vulkan_render_state *renderState) {
  vulkan_unified_geometry_buffer_send_to_device(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_send_to_device(renderState->unifiedUniformBuffer);

  assert(renderState->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderState->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderState->unifiedUniformBuffer->buffer->resident);
}

void vulkan_render_state_debug_print(vulkan_render_state *renderState) {
  log_debug("RENDER STATE:\n");
  vulkan_batches_debug_print(renderState->batches);
  vulkan_unified_geometry_buffer_debug_print(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderState->unifiedUniformBuffer);
}

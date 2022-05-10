#include "render_state.h"

vulkan_render_state *vulkan_render_state_create(vulkan_swap_chain *vks,
                                                vulkan_render_cache_list *renderCacheList,
                                                data_config *config) {
  assert(utarray_len(renderCacheList->primitiveRenderCaches) > 0);

  vulkan_render_state *renderState = core_alloc(sizeof(vulkan_render_state));

  renderState->vks = vks;
  renderState->vkd = renderState->vks->vkd;

  renderState->renderCacheList = renderCacheList;
  renderState->config = config;
  renderState->vertexStream = vulkan_vertex_stream_create(renderState->renderCacheList);

  renderState->unifiedGeometryBuffer =
      vulkan_unified_geometry_buffer_create(renderState->vkd, renderState->renderCacheList);
  renderState->textures = vulkan_textures_create(renderState->vkd);
  renderState->unifiedUniformBuffer =
      vulkan_unified_uniform_buffer_create(renderState->vkd, renderState->renderCacheList);
  renderState->descriptors = vulkan_descriptors_create(
      renderState->vkd, renderState->unifiedUniformBuffer, renderState->textures);
  renderState->sync = vulkan_sync_create(renderState->vkd);

  // HIRO HIRO HIRO refactor camera + light to CPU+GPU mixed state
  renderState->camera = vulkan_camera_create(renderState->renderCacheList, renderState->vks);
  renderState->lights = vulkan_lights_create(renderState->renderCacheList);
  renderState->skybox = vulkan_skybox_create(renderState);
  renderState->batches = vulkan_batches_create(renderCacheList, renderState->vkd);

  vulkan_render_state_update(renderState);

  return renderState;
}

void vulkan_render_state_destroy(vulkan_render_state *renderState) {
  vulkan_sync_destroy(renderState->sync);

  vulkan_descriptors_destroy(renderState->descriptors);

  vulkan_unified_uniform_buffer_destroy(renderState->unifiedUniformBuffer);
  vulkan_textures_destroy(renderState->textures);
  vulkan_unified_geometry_buffer_destroy(renderState->unifiedGeometryBuffer);

  vulkan_batches_destroy(renderState->batches);
  vulkan_skybox_destroy(renderState->skybox);
  vulkan_lights_destroy(renderState->lights);
  vulkan_camera_destroy(renderState->camera);

  vulkan_vertex_stream_destroy(renderState->vertexStream);

  core_free(renderState);
}

void vulkan_render_state_update(vulkan_render_state *renderState) {
  vulkan_render_cache_list_update_textures(renderState->renderCacheList, renderState->textures);
  vulkan_vertex_stream_update(renderState->vertexStream);

  vulkan_camera_update(renderState->camera);
  // HIRO Refactor vulkan_lights_update();

  vulkan_batch_instancing_policy batchPolicy =
      renderState->config->asset.graphicsEnabledInstancing
          ? vulkan_batch_instancing_policy_matching_vertex_attributes
          : vulkan_batch_instancing_policy_no_instancing;
  vulkan_batches_update(renderState->batches, batchPolicy);
  dl_foreach_elem(vulkan_batch *, batch, renderState->batches->batches) {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  }

  vulkan_unified_geometry_buffer_update(renderState->unifiedGeometryBuffer,
                                        renderState->vertexStream);
  vulkan_textures_update(renderState->textures);
  vulkan_unified_uniform_buffer_update(renderState->unifiedUniformBuffer, renderState->sync,
                                       renderState->camera, renderState->lights,
                                       renderState->skybox);

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
  vulkan_batches_debug_print(renderState->batches);
  vulkan_camera_debug_print(renderState->camera, 2);
  vulkan_lights_debug_print(renderState->lights, 2);
  vulkan_skybox_debug_print(renderState->skybox, 2);
  vulkan_unified_geometry_buffer_debug_print(renderState->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderState->unifiedUniformBuffer);
  vulkan_textures_debug_print(renderState->textures, 2);
  vulkan_descriptors_debug_print(renderState->descriptors, 2);
  vulkan_sync_debug_print(renderState->sync, 2);
}

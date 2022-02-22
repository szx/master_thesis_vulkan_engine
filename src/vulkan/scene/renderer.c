#include "renderer.h"

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_device *vkd,
                                                    UT_string *sceneName) {
  vulkan_scene_renderer *renderer = core_alloc(sizeof(vulkan_scene_renderer));
  data_config *config = data_config_create();

  renderer->assetDb = assetDb;
  renderer->vkd = vkd;
  renderer->data = vulkan_data_scene_create_with_asset_db(renderer->assetDb, sceneName);
  renderer->renderCacheList = vulkan_render_cache_list_create(config->graphicsMaxInstanceCount);
  renderer->batches = vulkan_batches_create(renderer->renderCacheList);
  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data, renderer->renderCacheList);

  vulkan_data_camera_init(&renderer->camera, renderer->data);
  vulkan_data_camera_copy(&renderer->camera, utarray_back(renderer->data->cameras));

  utarray_back(renderer->data->cameras);

  renderer->unifiedGeometryBuffer = vulkan_unified_geometry_buffer_create(vkd);
  renderer->unifiedUniformBuffer =
      vulkan_unified_uniform_buffer_create(vkd, renderer->renderCacheList, &renderer->camera);
  vulkan_scene_renderer_build_geometry_buffer(renderer);

  data_config_destroy(config);
  return renderer;
}

void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer) {
  vulkan_unified_uniform_buffer_destroy(renderer->unifiedUniformBuffer);
  vulkan_unified_geometry_buffer_destroy(renderer->unifiedGeometryBuffer);
  vulkan_scene_graph_destroy(renderer->sceneGraph);
  vulkan_batches_destroy(renderer->batches);
  vulkan_render_cache_list_destroy(renderer->renderCacheList);
  vulkan_data_scene_destroy(renderer->data);
  core_free(renderer);
}

void vulkan_scene_renderer_build_geometry_buffer(vulkan_scene_renderer *renderer) {
  /* build batches from cache list */
  vulkan_batch_policy batchPolicy = vulkan_batch_policy_matching_materials;
  vulkan_batches_update(renderer->batches, batchPolicy);

  /* build unified geometry buffer from batches */
  vulkan_attribute_type attributes = renderer->renderCacheList->attributes;
  assert(attributes != vulkan_attribute_type_unknown);
  vulkan_interleaved_vertex_stream *stream = vulkan_interleaved_vertex_stream_create(attributes);
  vulkan_unified_geometry_buffer_set_interleaved_vertex_stream(renderer->unifiedGeometryBuffer,
                                                               stream);
  dl_foreach_elem(vulkan_batch *, batch, renderer->batches->batches, {
    assert(batch->firstCache->primitive != NULL);
    vulkan_interleaved_vertex_stream_add_primitive(stream, batch->firstCache);
  })
  vulkan_unified_geometry_buffer_send_to_device(renderer->unifiedGeometryBuffer);

  /* update batch draw commands with offsets to unified buffers */
  dl_foreach_elem(vulkan_batch *, batch, renderer->batches->batches, {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);
  })

  /* build unified uniform buffer from HIRO what exactly? global shader config? */
  vulkan_unified_uniform_buffer_send_to_device(renderer->unifiedUniformBuffer);

  assert(renderer->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderer->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderer->unifiedUniformBuffer->buffer->resident);
}

void vulkan_scene_renderer_update(vulkan_scene_renderer *renderer) {
  vulkan_unified_uniform_buffer_update(renderer->unifiedUniformBuffer);
}

void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer) {
  // assert(!renderer->data->dirty);
  vulkan_unified_geometry_buffer_send_to_device(renderer->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_send_to_device(renderer->unifiedUniformBuffer);
}

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer) {
  log_debug("SCENE RENDERER:\n");
  vulkan_data_scene_debug_print(renderer->data);
  vulkan_scene_graph_debug_print(renderer->sceneGraph);
  vulkan_batches_debug_print(renderer->batches);
  vulkan_unified_geometry_buffer_debug_print(renderer->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderer->unifiedUniformBuffer);
}

#include "renderer.h"

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_device *vkd,
                                                    UT_string *sceneName) {
  vulkan_scene_renderer *renderer = core_alloc(sizeof(vulkan_scene_renderer));

  renderer->assetDb = assetDb;
  renderer->vkd = vkd;
  renderer->data = vulkan_data_scene_create_with_asset_db(renderer->assetDb, sceneName);
  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data);
  renderer->batches = vulkan_batches_create(renderer->sceneGraph);

  renderer->unifiedGeometryBuffer = vulkan_unified_geometry_buffer_create(vkd);
  renderer->unifiedUniformBuffer = vulkan_unified_uniform_buffer_create(vkd);
  vulkan_scene_renderer_build_geometry_buffer(renderer);
  return renderer;
}

void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer) {
  vulkan_unified_uniform_buffer_destroy(renderer->unifiedUniformBuffer);
  vulkan_unified_geometry_buffer_destroy(renderer->unifiedGeometryBuffer);
  vulkan_batches_destroy(renderer->batches);
  vulkan_scene_graph_destroy(renderer->sceneGraph);
  vulkan_data_scene_destroy(renderer->data);
  core_free(renderer);
}

void vulkan_scene_renderer_build_geometry_buffer(vulkan_scene_renderer *renderer) {
  /* build batches from cache list */
  vulkan_batch_policy batchPolicy = vulkan_batch_policy_matching_materials;
  vulkan_batches_update(renderer->batches, batchPolicy);

  /* build unified geometry buffer from batches */
  vulkan_attribute_type attributes = renderer->sceneGraph->sceneTree->cacheList->attributes;
  assert(attributes != UnknownAttribute);
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

void vulkan_scene_renderer_update_data(vulkan_scene_renderer *renderer) {
  utarray_foreach_elem_it (vulkan_data_camera *, camera, renderer->data->cameras) {
    vulkan_unified_uniform_buffer_update_with_camera(renderer->unifiedUniformBuffer, camera);
  }

  // renderer->data->dirty = renderer->unifiedUniformBuffer->dirty;
}

void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer) {
  // assert(!renderer->data->dirty);
  vulkan_unified_geometry_buffer_send_to_device(renderer->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_send_to_device(renderer->unifiedUniformBuffer);
}

void vulkan_unified_uniform_buffer_update_with_camera(
    vulkan_unified_uniform_buffer *unifiedUniformBuffer, vulkan_data_camera *camera) {
  if (!camera->dirty) {
    return;
  }
  vec3 negativePosition;
  mat4 translationMat;
  glm_vec3_negate_to(camera->position, negativePosition);
  glm_translate_make(translationMat, negativePosition);
  mat4 rotationMat;
  glm_quat_mat4(camera->rotation, rotationMat);
  glm_mat4_mul(rotationMat, translationMat, unifiedUniformBuffer->data.viewMat);

  glm_perspective(camera->fovY, camera->aspectRatio, camera->nearZ, camera->farZ,
                  unifiedUniformBuffer->data.projMat);
  unifiedUniformBuffer->data.projMat[1][1] *= -1; // invert for Y+ pointing up
  camera->dirty = false;
  unifiedUniformBuffer->dirty = true;
}

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer) {
  log_debug("SCENE:\n");
  vulkan_data_scene_debug_print(renderer->data);
  vulkan_scene_graph_debug_print(renderer->sceneGraph);
  vulkan_batches_debug_print(renderer->batches);
  vulkan_unified_geometry_buffer_debug_print(renderer->unifiedGeometryBuffer);
  // HIRO unified uniform buffer debug print
}

#include "renderer.h"

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_device *vkd,
                                                    UT_string *sceneName) {
  vulkan_scene_renderer *renderer = core_alloc(sizeof(vulkan_scene_renderer));

  renderer->assetDb = assetDb;
  renderer->vkd = vkd;
  renderer->data = vulkan_data_scene_create_with_asset_db(renderer->assetDb, sceneName);
  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data);
  renderer->batches = vulkan_batches_create(renderer->sceneGraph);

  renderer->geometryBuffer = vulkan_geometry_buffer_create();
  renderer->uniformBuffer = vulkan_uniform_buffer_create(vkd);
  vulkan_scene_renderer_build_geometry_buffer(renderer);
  return renderer;
}

void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer) {
  vulkan_data_scene_destroy(renderer->data);
  vulkan_uniform_buffer_destroy(renderer->uniformBuffer);
  vulkan_geometry_buffer_destroy(renderer->geometryBuffer);
  core_free(renderer);
}

void vulkan_scene_renderer_build_geometry_buffer(vulkan_scene_renderer *renderer) {
  // TODO: Overlapping index buffers and vertex streams.
  // TODO: Free node resources after building scene.
  // HIRO rethink vulkan_scene_build_geometry_buffer: interleave attributes from vulkan_data_object
  // here?
  /*
  vulkan_scene_debug_print(scene);
  UT_array *data = scene->geometryBuffer->data;
  assert(utarray_len(data) == 0);
  size_t paddingValue = 0;
  for (size_t nodeIdx = 0; nodeIdx < core_array_count(scene->data->nodes); nodeIdx++) {
    log_debug("node:\n");
    vulkan_data_object *object = &scene->data->nodes.ptr[nodeIdx];
    vulkan_data_mesh_data *mesh = &node->mesh;
    for (size_t primIdx = 0; primIdx < core_array_count(mesh->primitives); primIdx++) {
      vulkan_data_primitive *primitive = &mesh->primitives.ptr[primIdx];
      // index buffer
      size_t indexBufferOffset = utarray_len(data);
      if ((indexBufferOffset % primitive->indexStride) != 0) {
        size_t remainder = (indexBufferOffset % primitive->indexStride);
        indexBufferOffset += remainder;
        for (size_t i = 0; i < remainder; i++) {
          utarray_push_back(data, &paddingValue);
        }
      }
      primitive->indexBufferOffset = indexBufferOffset;
      size_t indexBufferSize = primitive->indexStride * primitive->indexCount;
      utarray_resize(data, utarray_len(data) + indexBufferSize);
      void *geometryData = utarray_eltptr(data, utarray_len(data) - indexBufferSize);
      void *indexData = utarray_front(primitive->indexBuffer);
      memcpy(geometryData, indexData, indexBufferSize);
      // vertex stream
      size_t vertexStreamOffset = utarray_len(data);
      if ((vertexStreamOffset % primitive->vertexStride) != 0) {
        // TODO: Is it too much? Should we align vertex streams?
        size_t remainder = (vertexStreamOffset % primitive->vertexStride);
        vertexStreamOffset += remainder;
        for (size_t i = 0; i < remainder; i++) {
          utarray_push_back(data, &paddingValue);
        }
      }
      primitive->vertexStreamOffset = vertexStreamOffset;
      size_t vertexStreamSize = primitive->vertexStride * primitive->vertexCount;
      utarray_resize(data, utarray_len(data) + vertexStreamSize);
      geometryData = utarray_eltptr(data, utarray_len(data) - vertexStreamSize);
      void *vertexData = utarray_front(primitive->vertexStream);
      memcpy(geometryData, vertexData, vertexStreamSize);
    }
  }*/
  panic("vulkan_scene_build_geometry_buffer in not implemented"); // HIRO build buffers
}

void vulkan_scene_renderer_update_data(vulkan_scene_renderer *renderer) {
  utarray_foreach_elem_it (vulkan_data_camera *, camera, renderer->data->cameras) {
    vulkan_uniform_buffer_update_with_camera(renderer->uniformBuffer, camera);
  }

  // renderer->data->dirty = renderer->uniformBuffer->dirty;
}

void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer) {
  // assert(!renderer->data->dirty);
  vulkan_geometry_buffer_send_to_device(renderer->vkd, renderer->geometryBuffer);
  vulkan_uniform_buffer_send_to_device(renderer->uniformBuffer);
}

void vulkan_uniform_buffer_update_with_camera(vulkan_uniform_buffer *uniformBuffer,
                                              vulkan_data_camera *camera) {
  if (!camera->dirty) {
    return;
  }
  vec3 negativePosition;
  mat4 translationMat;
  glm_vec3_negate_to(camera->position, negativePosition);
  glm_translate_make(translationMat, negativePosition);
  mat4 rotationMat;
  glm_quat_mat4(camera->rotation, rotationMat);
  glm_mat4_mul(rotationMat, translationMat, uniformBuffer->data.viewMat);

  glm_perspective(camera->fovY, camera->aspectRatio, camera->nearZ, camera->farZ,
                  uniformBuffer->data.projMat);
  uniformBuffer->data.projMat[1][1] *= -1; // invert for Y+ pointing up
  camera->dirty = false;
  uniformBuffer->dirty = true;
}

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", utarray_len(renderer->geometryBuffer->data));
  vulkan_data_scene_debug_print(renderer->data);
}

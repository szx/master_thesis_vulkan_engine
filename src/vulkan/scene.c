#include "scene.h"

vulkan_scene *vulkan_scene_create(vulkan_device *vkd, UT_string *sceneName) {
  vulkan_scene *scene = core_alloc(sizeof(vulkan_scene));
  scene->vkd = vkd;
  panic("not implemented"); // HIRO
  return scene;
  /*scene->geometryBuffer = vulkan_geometry_buffer_create();
  scene->uniformBuffer = vulkan_uniform_buffer_create(vkd);
  vulkan_scene_build_geometry_buffer(scene);*/
}

void vulkan_scene_destroy(vulkan_scene *scene) {
  vulkan_scene_data_destroy(scene->data);
  vulkan_uniform_buffer_destroy(scene->uniformBuffer);
  vulkan_geometry_buffer_destroy(scene->geometryBuffer);
  core_free(scene);
}

void vulkan_scene_debug_print(vulkan_scene *scene) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", utarray_len(scene->geometryBuffer->data));
  vulkan_scene_debug_print(scene);
}

void vulkan_scene_build_geometry_buffer(vulkan_scene *scene) {
  // TODO: Overlapping index buffers and vertex streams.
  // TODO: Free node resources after building scene.
  log_debug("vulkan_scene_build_geometry_buffer");
  vulkan_scene_debug_print(scene);
  UT_array *data = scene->geometryBuffer->data;
  assert(utarray_len(data) == 0);
  size_t paddingValue = 0;
  for (size_t nodeIdx = 0; nodeIdx < core_array_count(scene->data->nodes); nodeIdx++) {
    log_debug("node:\n");
    vulkan_node *node = &scene->data->nodes.ptr[nodeIdx];
    vulkan_mesh *mesh = &node->mesh;
    for (size_t primIdx = 0; primIdx < core_array_count(mesh->primitives); primIdx++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives.ptr[primIdx];
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
  }
}

void vulkan_scene_update_data(vulkan_scene *scene) {
  vulkan_camera_update_uniform_buffer_data(scene->data->camera, scene->uniformBuffer);
  scene->data->dirty = scene->data->camera->dirty || scene->uniformBuffer->dirty;
}

void vulkan_scene_send_to_device(vulkan_scene *scene) {
  assert(!scene->data->dirty);
  vulkan_geometry_buffer_send_to_device(scene->vkd, scene->geometryBuffer);
  vulkan_uniform_buffer_send_to_device(scene->uniformBuffer);
}

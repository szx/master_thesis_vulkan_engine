#include "render_data.h"

vulkan_scene_render_data *
vulkan_scene_render_data_create(data_asset_db *assetDb, vulkan_device *vkd, UT_string *sceneName) {
  vulkan_scene_render_data *renderData = core_alloc(sizeof(vulkan_scene_render_data));
  renderData->assetDb = assetDb;
  renderData->vkd = vkd;
  renderData->data = vulkan_data_scene_create_with_asset_db(renderData->assetDb, sceneName);
  renderData->geometryBuffer = vulkan_geometry_buffer_create();
  renderData->uniformBuffer = vulkan_uniform_buffer_create(vkd);
  vulkan_scene_render_data_build_geometry_buffer(renderData);
  return renderData;
}

void vulkan_scene_render_data_destroy(vulkan_scene_render_data *renderData) {
  vulkan_data_scene_destroy(renderData->data);
  vulkan_uniform_buffer_destroy(renderData->uniformBuffer);
  vulkan_geometry_buffer_destroy(renderData->geometryBuffer);
  core_free(renderData);
}

void vulkan_scene_render_data_build_geometry_buffer(vulkan_scene_render_data *renderData) {
  // HIRO return after research into GPU-driven rendering.
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

void vulkan_scene_render_data_update_data(vulkan_scene_render_data *renderData) {
  vulkan_data_camera *camera = NULL;
  while ((camera = (utarray_next(renderData->data->cameras, camera)))) {
    vulkan_uniform_buffer_update_with_camera(renderData->uniformBuffer, camera);
  }

  renderData->data->dirty = renderData->uniformBuffer->dirty;
}

void vulkan_scene_render_data_send_to_device(vulkan_scene_render_data *renderData) {
  assert(!renderData->data->dirty);
  vulkan_geometry_buffer_send_to_device(renderData->vkd, renderData->geometryBuffer);
  vulkan_uniform_buffer_send_to_device(renderData->uniformBuffer);
}

void vulkan_scene_render_data_debug_print(vulkan_scene_render_data *renderData) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", utarray_len(renderData->geometryBuffer->data));
  vulkan_data_scene_debug_print(renderData->data);
}

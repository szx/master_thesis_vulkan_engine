#include "renderer_cache_elements.h"

/* primitive renderer cache element */

vulkan_renderer_cache_primitive_element *vulkan_renderer_cache_primitive_element_create(
    bool visible, mat4 transform, vulkan_asset_primitive *primitive, vulkan_aabb aabb) {
  assert(primitive != NULL);

  vulkan_renderer_cache_primitive_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_primitive_element));

  element->prev = NULL;
  element->next = NULL;

  element->visible = visible;
  glm_mat4_copy(transform, element->transform);
  element->primitive = primitive;
  element->aabb = aabb;

  element->vertexStreamElement = (vulkan_vertex_stream_element){0};
  element->instanceId = 0;
  element->materialElement = 0;

  element->_ownsPrimitive = false;

  return element;
}

vulkan_renderer_cache_primitive_element *
vulkan_renderer_cache_primitive_element_create_from_geometry(bool visible, mat4 transform,
                                                             uint32_t vertexCount,
                                                             uint32_t *indices, vec3 *positions,
                                                             vec3 *normals, vec3 *colors,
                                                             vec2 *texCoords) {

  vulkan_asset_primitive *primitive = core_alloc(sizeof(vulkan_asset_primitive));
  vulkan_asset_primitive_init(primitive, NULL);
  primitive->vertexCount = vertexCount;

  primitive->indices = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(primitive->indices, NULL);
  if (indices != NULL) {
    utarray_realloc(primitive->indices->data, sizeof(uint32_t));
    utarray_resize(primitive->indices->data, 36);
    core_memcpy(utarray_front(primitive->indices->data), indices,
                utarray_size(primitive->indices->data));
  }
  primitive->positions = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(primitive->positions, NULL);
  if (positions != NULL) {
    utarray_realloc(primitive->positions->data, sizeof(vec3));
    utarray_resize(primitive->positions->data, 36);
    core_memcpy(utarray_front(primitive->positions->data), positions,
                utarray_size(primitive->positions->data));
  }
  primitive->normals = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(primitive->normals, NULL);
  if (normals != NULL) {
    utarray_realloc(primitive->normals->data, sizeof(vec3));
    utarray_resize(primitive->normals->data, 36);
    core_memcpy(utarray_front(primitive->normals->data), normals,
                utarray_size(primitive->normals->data));
  }
  primitive->colors = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(primitive->colors, NULL);
  if (colors != NULL) {
    utarray_realloc(primitive->colors->data, sizeof(vec3));
    utarray_resize(primitive->colors->data, 36);
    core_memcpy(utarray_front(primitive->colors->data), colors,
                utarray_size(primitive->colors->data));
  }
  primitive->texCoords = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(primitive->texCoords, NULL);
  if (texCoords != NULL) {
    utarray_realloc(primitive->texCoords->data, sizeof(vec3));
    utarray_resize(primitive->texCoords->data, 36);
    core_memcpy(utarray_front(primitive->texCoords->data), texCoords,
                utarray_size(primitive->texCoords->data));
  }

  vulkan_aabb aabb = vulkan_asset_primitive_calculate_aabb(primitive);
  vulkan_aabb_mul_mat4(&aabb, transform);

  vulkan_renderer_cache_primitive_element *element =
      vulkan_renderer_cache_primitive_element_create(visible, transform, primitive, aabb);
  element->_ownsPrimitive = true;
  return element;
}

void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element) {
  if (element->_ownsPrimitive) {
    if (element->primitive->indices != NULL) {
      vulkan_asset_vertex_attribute_deinit(element->primitive->indices);
      core_free(element->primitive->indices);
    }
    if (element->primitive->positions != NULL) {
      vulkan_asset_vertex_attribute_deinit(element->primitive->positions);
      core_free(element->primitive->positions);
    }
    if (element->primitive->normals != NULL) {
      vulkan_asset_vertex_attribute_deinit(element->primitive->normals);
      core_free(element->primitive->normals);
    }
    if (element->primitive->colors != NULL) {
      vulkan_asset_vertex_attribute_deinit(element->primitive->colors);
      core_free(element->primitive->colors);
    }
    if (element->primitive->texCoords != NULL) {
      vulkan_asset_vertex_attribute_deinit(element->primitive->texCoords);
      core_free(element->primitive->texCoords);
    }
    vulkan_asset_primitive_deinit(element->primitive);
    core_free(element->primitive);
  }
  core_free(element);
}

void vulkan_renderer_cache_primitive_element_debug_print(
    vulkan_renderer_cache_primitive_element *element) {
  log_raw(stdout, "\"renderer cache primitive cache\\n%p\\n", element);
  log_raw(stdout, "visible: %d\\n", element->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(element->transform));
  log_raw(stdout, "primitive: %p\\n", element->primitive);
  log_raw(stdout, "firstIndexOffset: %zu\\n", element->vertexStreamElement.firstIndexOffset);
  log_raw(stdout, "firstVertexOffset: %zu\\n", element->vertexStreamElement.firstVertexOffset);
  log_raw(stdout, "instanceId: %zu", element->instanceId);
  if (element->materialElement != NULL) {
    log_raw(stdout, "materialId: %zu", element->materialElement->materialIdx);
  }
  log_raw(stdout, "\"; ");
}

/* camera renderer cache element */

vulkan_renderer_cache_camera_element *
vulkan_renderer_cache_camera_element_create(vulkan_asset_camera *camera, mat4 transform) {
  assert(camera != NULL);

  vulkan_renderer_cache_camera_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_camera_element));

  element->prev = NULL;
  element->next = NULL;

  glm_mat4_copy(transform, element->transform);
  element->camera = camera;

  return element;
}

void vulkan_renderer_cache_camera_element_destroy(vulkan_renderer_cache_camera_element *element) {
  core_free(element);
}

void vulkan_renderer_cache_camera_element_debug_print(
    vulkan_renderer_cache_camera_element *element) {
  log_raw(stdout, "\"renderer cache camera element\\n%p\\n", element);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(element->transform));
  log_raw(stdout, "cameraFovY %f\\n", element->camera->fovY);
  log_raw(stdout, "\"; ");
}

/* skybox renderer cache element */

vulkan_renderer_cache_skybox_element *
vulkan_renderer_cache_skybox_element_create(vulkan_asset_skybox *skybox) {
  assert(skybox != NULL);

  vulkan_renderer_cache_skybox_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_skybox_element));

  element->prev = NULL;
  element->next = NULL;

  element->skybox = skybox;

  vulkan_asset_material_init(&element->cubemapMaterial, NULL);
  element->cubemapMaterial.baseColorTexture = element->skybox->cubemapTexture;
  element->cubemapMaterial.metallicRoughnessTexture = element->skybox->cubemapTexture;

  return element;
}

void vulkan_renderer_cache_skybox_element_destroy(vulkan_renderer_cache_skybox_element *element) {
  vulkan_asset_material_deinit(&element->cubemapMaterial);
  core_free(element);
}

void vulkan_renderer_cache_skybox_element_debug_print(
    vulkan_renderer_cache_skybox_element *element) {
  log_raw(stdout, "\"renderer skybox skybox element\\n%p\\n", element);
  log_raw(stdout, "cubemapTexture %p\\n", element->skybox->cubemapTexture);
  log_raw(stdout, "\"; ");
}

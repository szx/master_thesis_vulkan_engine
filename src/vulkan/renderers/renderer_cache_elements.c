#include "renderer_cache_elements.h"

/* primitive renderer cache element */

vulkan_renderer_cache_primitive_element *vulkan_renderer_cache_primitive_element_create(
    vulkan_renderer_cache_primitive_element_source_type sourceType, bool visible, mat4 transform,
    vulkan_asset_primitive *primitive, vulkan_aabb aabb) {
  assert(primitive != NULL);

  vulkan_renderer_cache_primitive_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_primitive_element));

  element->sourceType = sourceType;

  element->prev = NULL;
  element->next = NULL;

  element->visible = visible;
  glm_mat4_copy(transform, element->transform);
  element->primitive = primitive;
  element->aabb = aabb;

  element->vertexStreamElement = (vulkan_vertex_stream_element){0};
  element->instanceId = 0;
  element->materialElement = 0;

  return element;
}

void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element) {
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

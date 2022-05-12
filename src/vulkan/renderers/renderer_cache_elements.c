#include "renderer_cache_elements.h"

/* primitive renderer cache element */

vulkan_renderer_cache_primitive_element *vulkan_renderer_cache_primitive_element_create() {
  vulkan_renderer_cache_primitive_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_primitive_element));

  element->prev = NULL;
  element->next = NULL;

  vulkan_renderer_cache_primitive_element_reset(element);
  element->vertexStreamElement = (vulkan_vertex_stream_element){0};

  element->instanceId = 0;

  element->materialElement = 0;

  return element;
}

void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element) {
  core_free(element);
}

void vulkan_renderer_cache_primitive_element_reset(
    vulkan_renderer_cache_primitive_element *element) {
  element->distanceFromRoot = 0;
  element->visible = true;
  glm_mat4_identity(element->transform);
  element->mesh = NULL;
  element->primitive = NULL;
  element->aabb = vulkan_aabb_default();
}

void vulkan_renderer_cache_primitive_element_debug_print(
    vulkan_renderer_cache_primitive_element *element) {
  log_raw(stdout, "\"renderer cache primitive cache\\n%p\\n", element);
  log_raw(stdout, "distanceFromRoot: %d\\n", element->distanceFromRoot);
  log_raw(stdout, "visible: %d\\n", element->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(element->transform));
  log_raw(stdout, "mesh: %p\\n", element->mesh);
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

vulkan_renderer_cache_camera_element *vulkan_renderer_cache_camera_element_create() {
  vulkan_renderer_cache_camera_element *element =
      core_alloc(sizeof(vulkan_renderer_cache_camera_element));

  element->prev = NULL;
  element->next = NULL;

  glm_mat4_identity(element->transform);

  vulkan_asset_camera_init(&element->camera, NULL);

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
  log_raw(stdout, "cameraFovY %f\\n", element->camera.fovY);
  log_raw(stdout, "\"; ");
}

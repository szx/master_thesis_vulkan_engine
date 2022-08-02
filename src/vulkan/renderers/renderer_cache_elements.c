#include "renderer_cache_elements.h"

/* primitive renderer cache element */

renderer_cache_primitive_element *
renderer_cache_primitive_element_create(renderer_cache_primitive_element_source_type sourceType,
                                        bool visible, mat4 transform, asset_primitive *primitive,
                                        aabb aabb) {
  assert(primitive != NULL);

  renderer_cache_primitive_element *element = core_alloc(sizeof(renderer_cache_primitive_element));

  element->sourceType = sourceType;

  element->prev = NULL;
  element->next = NULL;

  element->visible = visible;
  glm_mat4_copy(transform, element->transform);
  element->primitive = primitive;
  element->aabb = aabb;

  element->vertexStreamElement = (vertex_stream_element){0};
  element->instanceId = 0;
  element->materialElement = 0;

#if defined(DEBUG)
  element->_instanceIdSet = false;
  element->_vertexStreamElementSet = false;
  element->_materialElementSet = false;
#endif

  return element;
}

void renderer_cache_primitive_element_destroy(renderer_cache_primitive_element *element) {
  core_free(element);
}

void renderer_cache_primitive_set_vertex_stream_element(renderer_cache_primitive_element *element,
                                                        vertex_stream_element vertexStreamElement) {
#if defined(DEBUG)
  assert(!element->_vertexStreamElementSet);
  element->_vertexStreamElementSet = true;
#endif
  element->vertexStreamElement = vertexStreamElement;
}

void renderer_cache_primitive_set_instance_id(renderer_cache_primitive_element *element,
                                              size_t instanceId) {
#if defined(DEBUG)
  element->_instanceIdSet = true;
#endif
  element->instanceId = instanceId;
}

void renderer_cache_primitive_set_material_element(renderer_cache_primitive_element *element,
                                                   textures_material_element *materialElement) {
  assert(materialElement != NULL);
#if defined(DEBUG)
  element->_materialElementSet = true;
#endif
  element->materialElement = materialElement;
}

void renderer_cache_primitive_element_debug_print(renderer_cache_primitive_element *element) {
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

renderer_cache_camera_element *renderer_cache_camera_element_create(asset_camera *camera,
                                                                    mat4 transform) {
  assert(camera != NULL);

  renderer_cache_camera_element *element = core_alloc(sizeof(renderer_cache_camera_element));

  element->prev = NULL;
  element->next = NULL;

  glm_mat4_copy(transform, element->transform);
  element->camera = camera;

  return element;
}

void renderer_cache_camera_element_destroy(renderer_cache_camera_element *element) {
  core_free(element);
}

void renderer_cache_camera_element_debug_print(renderer_cache_camera_element *element) {
  log_raw(stdout, "\"renderer cache camera element\\n%p\\n", element);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(element->transform));
  log_raw(stdout, "cameraFovY %f\\n", element->camera->fovY);
  log_raw(stdout, "\"; ");
}

renderer_cache_direct_light_element *
renderer_cache_direct_light_element_create(asset_direct_light *directLight) {
  assert(directLight != NULL);

  renderer_cache_direct_light_element *element =
      core_alloc(sizeof(renderer_cache_direct_light_element));

  element->prev = NULL;
  element->next = NULL;

  element->directLight = directLight;

  return element;
}

void renderer_cache_direct_light_element_destroy(renderer_cache_direct_light_element *element) {
  core_free(element);
}

void renderer_cache_direct_light_element_debug_print(renderer_cache_direct_light_element *element) {
  log_raw(stdout, "\"renderer cache direct light element\\n%p\\n", element);
  log_raw(stdout, "\"; ");
}

/* skybox renderer cache element */

renderer_cache_skybox_element *renderer_cache_skybox_element_create(asset_skybox *skybox) {
  assert(skybox != NULL);

  renderer_cache_skybox_element *element = core_alloc(sizeof(renderer_cache_skybox_element));

  element->skybox = skybox;
  element->skyboxTextureElement = NULL;

  return element;
}

void renderer_cache_skybox_element_destroy(renderer_cache_skybox_element *element) {
  core_free(element);
}

void renderer_cache_skybox_element_debug_print(renderer_cache_skybox_element *element) {
  log_raw(stdout, "\"renderer cache skybox element\\n%p\\n", element);
  log_raw(stdout, "cubemapTexture %p\\n", element->skybox->cubemapTexture);
  log_raw(stdout, "\"; ");
}

/* font renderer cache element */

renderer_cache_font_element *renderer_cache_font_element_create(asset_font *font) {
  assert(font != NULL);

  renderer_cache_font_element *element = core_alloc(sizeof(renderer_cache_font_element));

  element->font = font;
  element->fontTextureElement = NULL;

  return element;
}

void renderer_cache_font_element_destroy(renderer_cache_font_element *element) {
  core_free(element);
}

void renderer_cache_font_element_debug_print(renderer_cache_font_element *element) {
  log_raw(stdout, "\"renderer cache font element\\n%p\\n", element);
  log_raw(stdout, "fontTexture %p\\n", element->font->fontTexture);
  log_raw(stdout, "\"; ");
}

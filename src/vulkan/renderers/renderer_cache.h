/* Vulkan renderer cache.
 * Gathers information about rendered workload in form of various renderer cache elements.
 * Populated by:
 *  - scene tree
 *  - scene data
 *    - skybox
 *    - debug text
 *    - lights
 * Used by render graph to build draw calls.
 */
#pragma once

#include "../assets/assets.h"
#include "../scene/data.h"
#include "batch.h"
#include "renderer_cache_elements.h"

typedef struct renderer_cache {
  scene_data *sceneData; ///< Pointer.

  size_t maxPrimitiveElementCount; ///< Max number of renderer cache primitive elements.

  /// Linked list of all renderer cache primitive elements.
  renderer_cache_primitive_element *primitiveElements;

  vertex_attribute_type attributes; ///< Max set of primitive vertex attributes.

  aabb aabb; ///< World-space AABB derived from primitive's transform and model-space AABB.

  /* renderer cache camera elements accumulated from scene tree */
  /// Linked list of all renderer cache camera elements.
  renderer_cache_camera_element *cameraElements;

  /* additional state accumulated from scene data */
  renderer_cache_camera_element *defaultCameraElement;
  renderer_cache_direct_light_element *directLightElements;
  renderer_cache_skybox_element *skyboxElement;
  renderer_cache_font_element *fontElement;

  /* additional state set up during render cache creation */
  renderer_cache_primitive_element *basicBoxPrimitiveElement;
  renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement;

  bool _primitiveElementsDirty;
  UT_array *_newPrimitiveElements; ///< renderer_cache_primitive_element* array

} renderer_cache;

renderer_cache *renderer_cache_create(scene_data *sceneData, size_t maxPrimitiveElementCount);
void renderer_cache_destroy(renderer_cache *rendererCache);

void renderer_cache_add_primitive_element(renderer_cache *rendererCache,
                                          renderer_cache_primitive_element *primitiveElement);

void renderer_cache_calculate_aabb_for_primitive_elements(renderer_cache *rendererCache);

void renderer_cache_update_geometry(renderer_cache *rendererCache, vertex_stream *vertexStream);

void renderer_cache_update_textures(renderer_cache *rendererCache, textures *textures);

void renderer_cache_add_new_primitive_elements_to_batches(
    renderer_cache *rendererCache, batches *batches,
    renderer_cache_primitive_element_source_type sourceType);

void renderer_cache_add_camera_element(renderer_cache *rendererCache,
                                       renderer_cache_camera_element *cameraElement);

void renderer_cache_add_direct_light_element(
    renderer_cache *rendererCache, renderer_cache_direct_light_element *directLightElement);

void renderer_cache_remove_direct_light_element(
    renderer_cache *rendererCache, renderer_cache_direct_light_element *directLightElement);

void renderer_cache_add_skybox(renderer_cache *rendererCache,
                               renderer_cache_skybox_element *skyboxElement);

void renderer_cache_add_font(renderer_cache *rendererCache,
                             renderer_cache_font_element *fontElement);

void renderer_cache_debug_print(renderer_cache *rendererCache);

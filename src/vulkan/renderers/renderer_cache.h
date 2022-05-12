/* Vulkan renderer cache.
 * Gathers information about rendered workload in form of various renderer cache elements.
 * Populated by:
 *  - scene tree
 *  - scene data
 *    - skybox
 *    - HIRO lights
 * Used by renderer and pipelines to build draw calls.
 */
#pragma once

#include "../assets/texture.h"
#include "renderer_cache_elements.h"

// HIRO Refactor split vulkan_renderer_cache_element into multiple types (primitive, camera, other)
// HIRO
//  Refactor sort pointers to renderer cache primitive elements in batches (allows
//  transparent/opaque batches)
typedef struct vulkan_renderer_cache {

  size_t maxPrimitiveElementCount; ///< Max number of renderer cache primitive elements.

  /// Linked list of all renderer cache primitive elements.
  vulkan_renderer_cache_primitive_element *primitiveElements;

  // HIRO refactor remove primitiveElementsSorted, batches should be responsible
  bool primitiveElementsSorted; ///< True if primitiveElements is sorted.

  bool primitiveElementsDirty; ///< True if added cache.

  vulkan_attribute_type attributes; ///< Max set of primitive vertex attributes.

  vulkan_aabb aabb; ///< World-space AABB derived from primitive's transform and model-space AABB.

  /* renderer cache camera elements accumulated from scene tree */
  /// Linked list of all renderer cache camera elements.
  vulkan_renderer_cache_camera_element *cameraElements;

  /* additional state accumulated from scene data */
  vulkan_asset_skybox *skybox;

} vulkan_renderer_cache;

vulkan_renderer_cache *vulkan_renderer_cache_create(size_t maxPrimitiveElementCount);
void vulkan_renderer_cache_destroy(vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_add_primitive_element(
    vulkan_renderer_cache *rendererCache,
    vulkan_renderer_cache_primitive_element *primitiveElement);

void vulkan_renderer_cache_sort_primitive_elements(vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_calculate_aabb_for_primitive_elements(
    vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_update_geometry(vulkan_renderer_cache *rendererCache,
                                           vulkan_vertex_stream *vertexStream);

void vulkan_renderer_cache_update_textures(vulkan_renderer_cache *rendererCache,
                                           vulkan_textures *textures);

void vulkan_renderer_cache_add_camera_element(vulkan_renderer_cache *rendererCache,
                                              vulkan_renderer_cache_camera_element *cameraElement);

void vulkan_renderer_cache_add_skybox(vulkan_renderer_cache *rendererCache,
                                      vulkan_asset_skybox *skybox);

void vulkan_renderer_cache_debug_print(vulkan_renderer_cache *rendererCache);

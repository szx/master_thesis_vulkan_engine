/* Vulkan renderer cache.
 * Gathers information about rendered workload in form of various renderer cache elements.
 * Populated by by:
 *  - scene tree
 *  - scene data
 *    - skybox
 *    - HIRO lights
 * Used by pipelines to build draw calls.
 */
#pragma once

#include "../assets/texture.h"
#include "render_cache.h"

// HIRO Refactor split vulkan_render_cache into multiple types (primitive, camera, other)
// HIRO Refactor sort pointers to render caches in batches (allows transparent/opaque batches)
typedef struct vulkan_renderer_cache {
  /* primitive render caches accumulated from scene tree */
  size_t maxPrimitiveRenderCacheCount; ///< Max number of render caches.
  UT_array *primitiveRenderCaches; ///< vulkan_render_cache* array of primitive scene node caches.
  // HIRO refactor remove primitiveRenderCachesSorted, batches should be responsible
  bool primitiveRenderCachesSorted; ///< True if primitiveRenderCaches is sorted.
  bool primitiveRenderCachesDirty;  ///< True if added cache.
  vulkan_attribute_type attributes; ///< Max set of primitive vertex attributes.
  vulkan_aabb aabb; ///< World-space AABB derived from primitive's transform and model-space AABB.

  /* camera render caches accumulated from scene tree */
  UT_array *cameraRenderCaches; ///< vulkan_render_cache* array of camera scene node caches.

  /* primitive render caches accumulated from pipeline shared data (boxes, UI, etc.) */
  // HIRO refactor remove other render caches
  UT_array *otherRenderCaches; ///< vulkan_render_cache* array of render caches.
  bool otherRenderCachesDirty; ///< True if added cache to otherRenderCaches

  /* additional state accumulated from scene data */
  vulkan_asset_skybox *skybox;

} vulkan_renderer_cache;

vulkan_renderer_cache *vulkan_renderer_cache_create(size_t maxPrimitiveRenderCacheCount);
void vulkan_renderer_cache_destroy(vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_add_primitive_render_cache(vulkan_renderer_cache *rendererCache,
                                                      vulkan_render_cache *primitiveRenderCache);

void vulkan_renderer_cache_sort_primitive_render_caches(vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_calculate_aabb_for_primitive_render_caches(
    vulkan_renderer_cache *rendererCache);

void vulkan_renderer_cache_update_geometry(vulkan_renderer_cache *rendererCache,
                                           vulkan_vertex_stream *vertexStream);

void vulkan_renderer_cache_update_textures(vulkan_renderer_cache *rendererCache,
                                           vulkan_textures *textures);

void vulkan_renderer_cache_add_camera_render_cache(vulkan_renderer_cache *rendererCache,
                                                   vulkan_render_cache *cameraRenderCache);

void vulkan_renderer_cache_add_other_render_cache(vulkan_renderer_cache *rendererCache,
                                                  vulkan_render_cache *otherRenderCache);

void vulkan_renderer_cache_add_skybox(vulkan_renderer_cache *rendererCache,
                                      vulkan_asset_skybox *skybox);

void vulkan_renderer_cache_debug_print(vulkan_renderer_cache *rendererCache);

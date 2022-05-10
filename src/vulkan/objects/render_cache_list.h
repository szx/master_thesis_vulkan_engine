/* Vulkan scene cache list.
 * Tracks scene tree nodes that are:
 *  - primitives (leaves of scene tree),
 *  - cameras,
 * Tracks scene data of:
 *  - skybox,
 * Used by pipelines to build draw calls.
 */
#pragma once

#include "../assets/texture.h"
#include "render_cache.h"

// HIRO Refactor rename to vulkan_renderer_cache
typedef struct vulkan_render_cache_list {
  /* primitive render caches accumulated from scene tree */
  size_t maxPrimitiveRenderCacheCount; ///< Max number of render caches.
  UT_array *primitiveRenderCaches;  ///< vulkan_render_cache* array of primitive scene node caches.
  bool primitiveRenderCachesSorted; ///< True if primitiveRenderCaches is sorted.
  vulkan_attribute_type attributes; ///< Max set of primitive vertex attributes.
  vulkan_aabb aabb; ///< World-space AABB derived from primitive's transform and model-space AABB.

  /* camera render caches accumulated from scene tree */
  UT_array *cameraRenderCaches; ///< vulkan_render_cache* array of camera scene node caches.

  /* additional state accumulated from scene data */
  // HIRO HIRO Refactor replace with skyboxRenderCache
  vulkan_asset_skybox *skybox;

} vulkan_render_cache_list;

vulkan_render_cache_list *vulkan_render_cache_list_create(size_t maxPrimitiveRenderCacheCount);
void vulkan_render_cache_list_destroy(vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_add_primitive_render_cache(vulkan_render_cache_list *renderCacheList,
                                                         vulkan_render_cache *primitiveRenderCache);

void vulkan_render_cache_list_sort_primitive_render_caches(
    vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_calculate_aabb_for_primitive_render_caches(
    vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_update_textures(vulkan_render_cache_list *renderCacheList,
                                              vulkan_textures *textures);

void vulkan_render_cache_list_add_camera_render_cache(vulkan_render_cache_list *renderCacheList,
                                                      vulkan_render_cache *cameraRenderCache);

void vulkan_render_cache_list_add_skybox(vulkan_render_cache_list *renderCacheList,
                                         vulkan_asset_skybox *skybox);

void vulkan_render_cache_list_debug_print(vulkan_render_cache_list *renderCacheList);

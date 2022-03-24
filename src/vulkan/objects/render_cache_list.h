/* Vulkan scene cache list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Used to build draw call batches.
 */
#pragma once

#include "render_cache.h"

typedef struct vulkan_render_cache_list {
  /* primitive render caches */
  size_t maxPrimitiveRenderCacheCount; ///< Max number of render caches.
  UT_array *primitiveRenderCaches;  ///< vulkan_render_cache* array of primitive scene node caches.
  vulkan_attribute_type attributes; ///< Max set of primitive vertex attributes.

  /* camera render caches */
  size_t cameraRenderCacheCount; ///< Number of camera render caches.
  UT_array *cameraRenderCaches;  ///< vulkan_render_cache* array of camera scene node caches.

  bool dirty; ///< True if added or removed cache and not sorted.
} vulkan_render_cache_list;

vulkan_render_cache_list *vulkan_render_cache_list_create(size_t maxPrimitiveRenderCacheCount);
void vulkan_render_cache_list_destroy(vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_add_primitive_render_cache(vulkan_render_cache_list *renderCacheList,
                                                         vulkan_render_cache *primitiveRenderCache);

void vulkan_render_cache_list_remove_primitive_render_cache(
    vulkan_render_cache_list *renderCacheList, vulkan_render_cache *primitiveRenderCache);

void vulkan_render_cache_list_add_camera_render_cache(vulkan_render_cache_list *renderCacheList,
                                                      vulkan_render_cache *cameraRenderCache);

/// Sorts cache list in a way that minimizes number of created batches.
/// Updates max set of vertex attributes.
void vulkan_render_cache_list_update(vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_debug_print(vulkan_render_cache_list *renderCacheList);

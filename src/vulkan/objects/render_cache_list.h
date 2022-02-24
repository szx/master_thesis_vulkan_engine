/* Vulkan scene cache list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Used to build draw call batches.
 */
#pragma once

#include "render_cache.h"

typedef struct vulkan_render_cache_list {
  /* render cache list */
  size_t maxCount;                  /// Max number of render caches.
  UT_array *caches;                 /// vulkan_render_cache* array of primitive scene node caches.
  vulkan_attribute_type attributes; /// Max set of primitive vertex attributes.

  bool dirty; /// True if added or removed cache.
} vulkan_render_cache_list;

vulkan_render_cache_list *vulkan_render_cache_list_create(size_t maxCount);
void vulkan_render_cache_list_destroy(vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_add_cache(vulkan_render_cache_list *renderCacheList,
                                        vulkan_render_cache *cache);

void vulkan_render_cache_list_remove_cache(vulkan_render_cache_list *renderCacheList,
                                           vulkan_render_cache *cache);

/// Sorts cache list in a way that minimizes number of created batches.
/// Updates max set of vertex attributes.
void vulkan_render_cache_list_update(vulkan_render_cache_list *renderCacheList);

void vulkan_render_cache_list_debug_print(vulkan_render_cache_list *renderCacheList);

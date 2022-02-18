/* Vulkan scene cache list.
 * Tracks primitive scene nodes that are leaves of scene tree.
 * Used to build draw call batches.
 */
#pragma once

#include "tree.h"

typedef struct vulkan_batches vulkan_batches;

typedef struct vulkan_scene_cache_list {
  vulkan_scene_tree *sceneTree; /// vulkan_scene_tree pointer.

  UT_array *caches;                 /// vulkan_scene_cache* array of primitive scene node caches.
  vulkan_attribute_type attributes; /// Max set of primitive vertex attributes.

  bool dirty; /// True if added or removed cache.
} vulkan_scene_cache_list;

vulkan_scene_cache_list *vulkan_scene_cache_list_create(vulkan_scene_tree *sceneTree);
void vulkan_scene_cache_list_destroy(vulkan_scene_cache_list *cacheList);

void vulkan_scene_cache_list_add_cache(vulkan_scene_cache_list *cacheList,
                                       vulkan_scene_cache *cache);

void vulkan_scene_cache_list_remove_cache(vulkan_scene_cache_list *cacheList,
                                          vulkan_scene_cache *cache);

/// Sorts cache list in a way that minimizes number of created batches.
/// Updates max set of vertex attributes.
void vulkan_scene_cache_list_sort_and_update(vulkan_scene_cache_list *cacheList);

void vulkan_scene_cache_list_debug_print(vulkan_scene_cache_list *cacheList);

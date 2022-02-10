/* Vulkan scene cache.
 * Represents scene node state that has been accumulated while traversing
 * from scene node to its successors.
 * Used to:
 * accumulate scene node state changes propagated from dirty scene node,
 * build render data from primitive scene nodes.
 */
#pragma once

#include "../data/scene.h"

typedef struct vulkan_scene_node vulkan_scene_node;

typedef struct vulkan_scene_cache {
  vulkan_scene_node *node;
  /* scene node state */
  bool visible;
  mat4 transform; /// Accumulated from object data.
} vulkan_scene_cache;

vulkan_scene_cache *vulkan_scene_cache_create(vulkan_scene_node *sceneNode);
void vulkan_scene_cache_destroy(vulkan_scene_cache *sceneCache);

// HIRO accumulate_from_node

void vulkan_scene_cache_debug_print(vulkan_scene_cache *sceneCache);

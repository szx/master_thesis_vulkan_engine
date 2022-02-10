#include "cache.h"

vulkan_scene_cache *vulkan_scene_cache_create(vulkan_scene_node *sceneNode) {
  vulkan_scene_cache *sceneCache = core_alloc(sizeof(vulkan_scene_cache));

  sceneCache->node = sceneNode;
  sceneCache->visible = true;

  return sceneCache;
}

void vulkan_scene_cache_destroy(vulkan_scene_cache *sceneCache) { core_free(sceneCache); }

void debug_log_cache(vulkan_scene_cache *sceneCache) {
  log_raw(stdout, "\"cache\\n%p\\n", sceneCache->node);
  log_raw(stdout, "visible_%d\\n", sceneCache->visible);
  log_raw(stdout, "\"");
}

void vulkan_scene_cache_debug_print(vulkan_scene_cache *sceneCache) {
  debug_log_cache(sceneCache);
  log_raw(stdout, "; ");
}

#include "cache.h"
#include "node.h"

vulkan_scene_cache *vulkan_scene_cache_create(vulkan_scene_node *sceneNode) {
  vulkan_scene_cache *sceneCache = core_alloc(sizeof(vulkan_scene_cache));

  sceneCache->node = sceneNode;
  sceneCache->visible = true;
  if (sceneCache->node->type == vulkan_scene_node_type_object) {
    glm_mat4_copy(sceneCache->node->object->transform, sceneCache->transform);
  } else {
    glm_mat4_identity(sceneCache->transform);
  }
  return sceneCache;
}

void vulkan_scene_cache_destroy(vulkan_scene_cache *sceneCache) { core_free(sceneCache); }

void debug_log_cache(vulkan_scene_cache *sceneCache) {
  log_raw(stdout, "\"cache\\n%p\\n", sceneCache->node);
  log_raw(stdout, "visible: %d\\n", sceneCache->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(sceneCache->transform));
  log_raw(stdout, "\"");
}

void vulkan_scene_cache_debug_print(vulkan_scene_cache *sceneCache) {
  debug_log_cache(sceneCache);
  log_raw(stdout, "; ");
}

#include "cache.h"
#include "node.h"

vulkan_scene_cache *vulkan_scene_cache_create(vulkan_scene_node *sceneNode) {
  vulkan_scene_cache *sceneCache = core_alloc(sizeof(vulkan_scene_cache));

  sceneCache->node = sceneNode;
  vulkan_scene_cache_set_with_node(sceneCache);

  return sceneCache;
}

void vulkan_scene_cache_destroy(vulkan_scene_cache *sceneCache) { core_free(sceneCache); }

void vulkan_scene_cache_set_with_node(vulkan_scene_cache *sceneCache) {
  sceneCache->distanceFromRoot = 0;
  sceneCache->visible = true;
  if (sceneCache->node->type == vulkan_scene_node_entity_type_object) {
    glm_mat4_copy(sceneCache->node->object->transform, sceneCache->transform);
  } else {
    glm_mat4_identity(sceneCache->transform);
  }
}

void vulkan_scene_cache_accumulate(vulkan_scene_cache *sceneCache,
                                   vulkan_scene_cache *parentCache) {
  vulkan_scene_cache_set_with_node(sceneCache);
  sceneCache->distanceFromRoot = parentCache->distanceFromRoot + 1;
  sceneCache->visible = parentCache->visible;
  glm_mat4_mul(parentCache->transform, sceneCache->transform, sceneCache->transform);
}

void debug_log_cache(vulkan_scene_cache *sceneCache) {
  log_raw(stdout, "\"cache\\n%p\\n", sceneCache->node);
  log_raw(stdout, "distanceFromRoot: %d\\n", sceneCache->distanceFromRoot);
  log_raw(stdout, "visible: %d\\n", sceneCache->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(sceneCache->transform));
  log_raw(stdout, "\"");
}

void vulkan_scene_cache_debug_print(vulkan_scene_cache *sceneCache) {
  debug_log_cache(sceneCache);
  log_raw(stdout, "; ");
}

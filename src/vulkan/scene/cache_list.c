#include "cache_list.h"

vulkan_scene_cache_list *vulkan_scene_cache_list_create(vulkan_scene_tree *sceneTree) {
  vulkan_scene_cache_list *primitiveList = core_alloc(sizeof(vulkan_scene_cache_list));

  primitiveList->sceneTree = sceneTree;
  utarray_alloc(primitiveList->caches, sizeof(vulkan_scene_node *));
  primitiveList->dirty = false;

  return primitiveList;
}

void vulkan_scene_cache_list_destroy(vulkan_scene_cache_list *primitiveList) {
  utarray_free(primitiveList->caches);
  core_free(primitiveList);
}

void vulkan_scene_cache_list_add_cache(vulkan_scene_cache_list *primitiveList,
                                       vulkan_scene_cache *cache) {
  utarray_push_back(primitiveList->caches, &cache);
  primitiveList->dirty = true;
}

void vulkan_scene_cache_list_remove_cache(vulkan_scene_cache_list *primitiveList,
                                          vulkan_scene_cache *cache) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_scene_cache *, existingCache, primitiveList->caches) {
    if (existingCache == cache) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(primitiveList->caches));
  utarray_erase(primitiveList->caches, idx, 1);
  primitiveList->dirty = true;
}

void vulkan_scene_cache_list_debug_print(vulkan_scene_cache_list *primitiveList) {
  log_raw(stdout, "digraph cache_list {");
  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, primitiveList->caches) {
    vulkan_scene_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");
}

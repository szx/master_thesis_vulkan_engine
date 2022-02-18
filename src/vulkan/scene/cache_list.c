#include "cache_list.h"

vulkan_scene_cache_list *vulkan_scene_cache_list_create(vulkan_scene_tree *sceneTree) {
  vulkan_scene_cache_list *cacheList = core_alloc(sizeof(vulkan_scene_cache_list));

  cacheList->sceneTree = sceneTree;
  utarray_alloc(cacheList->caches, sizeof(vulkan_scene_node *));
  cacheList->attributes = UnknownAttribute;

  cacheList->dirty = false;

  return cacheList;
}

void vulkan_scene_cache_list_destroy(vulkan_scene_cache_list *cacheList) {
  utarray_free(cacheList->caches);
  core_free(cacheList);
}

void vulkan_scene_cache_list_add_cache(vulkan_scene_cache_list *cacheList,
                                       vulkan_scene_cache *cache) {
  utarray_push_back(cacheList->caches, &cache);
  cacheList->dirty = true;
}

void vulkan_scene_cache_list_remove_cache(vulkan_scene_cache_list *cacheList,
                                          vulkan_scene_cache *cache) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_scene_cache *, existingCache, cacheList->caches) {
    if (existingCache == cache) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(cacheList->caches));
  utarray_erase(cacheList->caches, idx, 1);
  cacheList->dirty = true;
}

static int cache_list_sort_func(const void *_a, const void *_b) {
  const vulkan_scene_cache *a = *(const vulkan_scene_cache **)_a;
  const vulkan_scene_cache *b = *(const vulkan_scene_cache **)_b;
  // sort by visibility
  if (a->visible && !b->visible) {
    return -1;
  }
  if (!a->visible && b->visible) {
    return 1;
  }
  if (!a->visible && !b->visible) {
    return 0; // Both invisible, do not care.
  }

  if (a->primitive == b->primitive) {
    return 0; // Same geometry and material, ideal for batching.
  }

  // sort by material
  if (a->primitive->material->key.value < b->primitive->material->key.value) {
    return -1;
  }
  if (a->primitive->material->key.value > b->primitive->material->key.value) {
    return 1;
  }

  // sort by geometry
  if (a->primitive->geometryHash < b->primitive->geometryHash) {
    return -1;
  }
  if (a->primitive->geometryHash > b->primitive->geometryHash) {
    return 1;
  }
  return 0;
}

void vulkan_scene_cache_list_sort_and_update(vulkan_scene_cache_list *cacheList) {
  if (!cacheList->dirty) {
    return;
  }

  utarray_sort(cacheList->caches, cache_list_sort_func);

  cacheList->attributes = UnknownAttribute;
  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, cacheList->caches) {
    cacheList->attributes |= cache->primitive->attributes;
  }
}

void vulkan_scene_cache_list_debug_print(vulkan_scene_cache_list *cacheList) {
  log_raw(stdout, "digraph cache_list {");
  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, cacheList->caches) {
    vulkan_scene_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");
}

#include "render_cache_list.h"
#include "../data/primitive.h"

vulkan_render_cache_list *vulkan_render_cache_list_create() {
  vulkan_render_cache_list *renderCacheList = core_alloc(sizeof(vulkan_render_cache_list));

  utarray_alloc(renderCacheList->caches, sizeof(vulkan_scene_node *));
  renderCacheList->attributes = vulkan_attribute_type_unknown;

  renderCacheList->dirty = false;

  return renderCacheList;
}

void vulkan_render_cache_list_destroy(vulkan_render_cache_list *renderCacheList) {
  utarray_free(renderCacheList->caches);
  core_free(renderCacheList);
}

void vulkan_render_cache_list_add_cache(vulkan_render_cache_list *renderCacheList,
                                        vulkan_render_cache *cache) {
  utarray_push_back(renderCacheList->caches, &cache);
  renderCacheList->dirty = true;
}

void vulkan_render_cache_list_remove_cache(vulkan_render_cache_list *renderCacheList,
                                           vulkan_render_cache *cache) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_render_cache *, existingCache, renderCacheList->caches) {
    if (existingCache == cache) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(renderCacheList->caches));
  utarray_erase(renderCacheList->caches, idx, 1);
  renderCacheList->dirty = true;
}

static int cache_list_sort_func(const void *_a, const void *_b) {
  const vulkan_render_cache *a = *(const vulkan_render_cache **)_a;
  const vulkan_render_cache *b = *(const vulkan_render_cache **)_b;
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

void vulkan_render_cache_list_sort_and_update(vulkan_render_cache_list *renderCacheList) {
  if (!renderCacheList->dirty) {
    return;
  }

  utarray_sort(renderCacheList->caches, cache_list_sort_func);

  renderCacheList->attributes = vulkan_attribute_type_unknown;
  utarray_foreach_elem_deref (vulkan_render_cache *, cache, renderCacheList->caches) {
    renderCacheList->attributes |= cache->primitive->attributes;
  }
}

void vulkan_render_cache_list_debug_print(vulkan_render_cache_list *renderCacheList) {
  log_raw(stdout, "digraph cache_list {");
  utarray_foreach_elem_deref (vulkan_render_cache *, cache, renderCacheList->caches) {
    vulkan_render_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");
}

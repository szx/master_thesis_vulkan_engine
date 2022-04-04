#include "render_cache_list.h"
#include "../data/primitive.h"

vulkan_render_cache_list *vulkan_render_cache_list_create(size_t maxInstanceCount) {
  vulkan_render_cache_list *renderCacheList = core_alloc(sizeof(vulkan_render_cache_list));

  renderCacheList->maxPrimitiveRenderCacheCount = maxInstanceCount;
  utarray_alloc(renderCacheList->primitiveRenderCaches, sizeof(vulkan_render_cache *));
  renderCacheList->attributes = vulkan_attribute_type_unknown;

  utarray_alloc(renderCacheList->cameraRenderCaches, sizeof(vulkan_render_cache *));

  renderCacheList->dirty = true;

  return renderCacheList;
}

void vulkan_render_cache_list_destroy(vulkan_render_cache_list *renderCacheList) {
  utarray_free(renderCacheList->cameraRenderCaches);
  utarray_free(renderCacheList->primitiveRenderCaches);
  core_free(renderCacheList);
}

void vulkan_render_cache_list_add_primitive_render_cache(
    vulkan_render_cache_list *renderCacheList, vulkan_render_cache *primitiveRenderCache) {
  assert(primitiveRenderCache->primitive != NULL);
  verify(utarray_len(renderCacheList->primitiveRenderCaches) <
         renderCacheList->maxPrimitiveRenderCacheCount);
  utarray_push_back(renderCacheList->primitiveRenderCaches, &primitiveRenderCache);

  renderCacheList->dirty = true;
}

void vulkan_render_cache_list_remove_primitive_render_cache(
    vulkan_render_cache_list *renderCacheList, vulkan_render_cache *primitiveRenderCache) {
  assert(primitiveRenderCache->primitive != NULL);
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_render_cache *, existingCache,
                              renderCacheList->primitiveRenderCaches) {
    if (existingCache == primitiveRenderCache) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(renderCacheList->primitiveRenderCaches));
  utarray_erase(renderCacheList->primitiveRenderCaches, idx, 1);
  renderCacheList->dirty = true;
}

void vulkan_render_cache_list_add_camera_render_cache(vulkan_render_cache_list *renderCacheList,
                                                      vulkan_render_cache *cameraRenderCache) {
  utarray_push_back(renderCacheList->cameraRenderCaches, &cameraRenderCache);
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

void vulkan_render_cache_list_update(vulkan_render_cache_list *renderCacheList) {
  if (!renderCacheList->dirty) {
    return;
  }
  log_debug("update");

  utarray_sort(renderCacheList->primitiveRenderCaches, cache_list_sort_func);

  renderCacheList->attributes = vulkan_attribute_type_unknown;
  utarray_foreach_elem_deref (vulkan_render_cache *, cache,
                              renderCacheList->primitiveRenderCaches) {
    renderCacheList->attributes |= cache->primitive->attributes;
  }

  renderCacheList->dirty = true;
}

void vulkan_render_cache_list_debug_print(vulkan_render_cache_list *renderCacheList) {
  log_raw(stdout, "digraph primitive_render_cache_list {");
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              renderCacheList->primitiveRenderCaches) {
    vulkan_render_cache_debug_print(primitiveRenderCache);
  }
  log_raw(stdout, "}\n");

  log_raw(stdout, "digraph camera_render_cache_list {");
  utarray_foreach_elem_deref (vulkan_render_cache *, cameraRenderCache,
                              renderCacheList->cameraRenderCaches) {
    vulkan_render_cache_debug_print(cameraRenderCache);
  }
  log_raw(stdout, "}\n");
}

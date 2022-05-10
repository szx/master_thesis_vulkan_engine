#include "render_cache_list.h"
#include "../assets/primitive.h"

vulkan_render_cache_list *vulkan_render_cache_list_create(size_t maxInstanceCount) {
  vulkan_render_cache_list *renderCacheList = core_alloc(sizeof(vulkan_render_cache_list));

  renderCacheList->maxPrimitiveRenderCacheCount = maxInstanceCount;
  utarray_alloc(renderCacheList->primitiveRenderCaches, sizeof(vulkan_render_cache *));
  renderCacheList->primitiveRenderCachesSorted = true;
  renderCacheList->attributes = vulkan_attribute_type_unknown;
  renderCacheList->aabb = vulkan_aabb_default();

  utarray_alloc(renderCacheList->cameraRenderCaches, sizeof(vulkan_render_cache *));

  renderCacheList->skybox = NULL;

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

  renderCacheList->primitiveRenderCachesSorted = false;

  renderCacheList->attributes |= primitiveRenderCache->primitive->attributes;

  // NOTE: Primitive render cache's AABB are is calculated in
  // vulkan_render_cache_list_sort_primitive_render_caches.

  // NOTE: Primitive render cache's textures are added in vulkan_render_cache_list_update_textures.
}

void vulkan_render_cache_list_update_textures(vulkan_render_cache_list *renderCacheList,
                                              vulkan_textures *textures) {
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              renderCacheList->primitiveRenderCaches) {
    assert(primitiveRenderCache->primitive != NULL);
    vulkan_asset_material *material = primitiveRenderCache->primitive->material;
    if (material == NULL) {
      continue;
    }
    vulkan_textures_material_element *element = vulkan_textures_add_material(textures, material);
    primitiveRenderCache->materialElement = element;

    // TODO: Unload unneeded textures.
  }
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

  // sort by geometry
  if (a->primitive->geometryHash < b->primitive->geometryHash) {
    return -1;
  }
  if (a->primitive->geometryHash > b->primitive->geometryHash) {
    return 1;
  }
  return 0;
}

void vulkan_render_cache_list_sort_primitive_render_caches(
    vulkan_render_cache_list *renderCacheList) {
  if (renderCacheList->primitiveRenderCachesSorted) {
    return;
  }
  log_debug("sorting primitive render caches");

  // sort primitive render caches for batching
  utarray_sort(renderCacheList->primitiveRenderCaches, cache_list_sort_func);

  renderCacheList->primitiveRenderCachesSorted = true;
}

void vulkan_render_cache_list_calculate_aabb_for_primitive_render_caches(
    vulkan_render_cache_list *renderCacheList) {
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              renderCacheList->primitiveRenderCaches) {
    vulkan_aabb_add_aabb(&renderCacheList->aabb, &primitiveRenderCache->aabb);
  }
}

void vulkan_render_cache_list_add_camera_render_cache(vulkan_render_cache_list *renderCacheList,
                                                      vulkan_render_cache *cameraRenderCache) {
  utarray_push_back(renderCacheList->cameraRenderCaches, &cameraRenderCache);
}

void vulkan_render_cache_list_add_skybox(vulkan_render_cache_list *renderCacheList,
                                         vulkan_asset_skybox *skybox) {
  renderCacheList->skybox = skybox;
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

  vulkan_aabb_debug_print(&renderCacheList->aabb, 2);
}

#include "renderer_cache.h"
#include "../assets/primitive.h"

vulkan_renderer_cache *vulkan_renderer_cache_create(size_t maxPrimitiveRenderCacheCount) {
  vulkan_renderer_cache *rendererCache = core_alloc(sizeof(vulkan_renderer_cache));

  rendererCache->maxPrimitiveRenderCacheCount = maxPrimitiveRenderCacheCount;
  utarray_alloc(rendererCache->primitiveRenderCaches, sizeof(vulkan_render_cache *));
  rendererCache->primitiveRenderCachesSorted = true;
  rendererCache->primitiveRenderCachesDirty = false;
  rendererCache->attributes = vulkan_attribute_type_unknown;
  rendererCache->aabb = vulkan_aabb_default();

  utarray_alloc(rendererCache->cameraRenderCaches, sizeof(vulkan_render_cache *));

  utarray_alloc(rendererCache->otherRenderCaches, sizeof(vulkan_render_cache *));
  rendererCache->otherRenderCachesDirty = false;

  rendererCache->skybox = NULL;

  return rendererCache;
}

void vulkan_renderer_cache_destroy(vulkan_renderer_cache *rendererCache) {
  utarray_free(rendererCache->otherRenderCaches);
  utarray_free(rendererCache->cameraRenderCaches);
  utarray_free(rendererCache->primitiveRenderCaches);
  core_free(rendererCache);
}

void vulkan_renderer_cache_add_primitive_render_cache(vulkan_renderer_cache *rendererCache,
                                                      vulkan_render_cache *primitiveRenderCache) {
  assert(primitiveRenderCache->primitive != NULL);
  verify(utarray_len(rendererCache->primitiveRenderCaches) <
         rendererCache->maxPrimitiveRenderCacheCount);

  utarray_push_back(rendererCache->primitiveRenderCaches, &primitiveRenderCache);

  rendererCache->primitiveRenderCachesSorted = false;
  rendererCache->primitiveRenderCachesDirty = true;

  rendererCache->attributes |= primitiveRenderCache->primitive->attributes;

  // NOTE: Primitive render cache's AABB are is calculated in
  // vulkan_renderer_cache_sort_primitive_render_caches.

  // NOTE: Primitive render cache's geometry is added in vulkan_renderer_cache_update_geometry.
  // NOTE: Primitive render cache's textures are added in vulkan_renderer_cache_update_textures.
}

void vulkan_renderer_cache_update_geometry(vulkan_renderer_cache *rendererCache,
                                           vulkan_vertex_stream *vertexStream) {
  if (!rendererCache->primitiveRenderCachesDirty) {
    return;
  }
  log_debug("updating rendererCache -> vulkan_vertex_stream");

  // Sort primitive render caches by geometry to minimize geometry buffer size.
  vulkan_renderer_cache_sort_primitive_render_caches(rendererCache);
  assert(rendererCache->primitiveRenderCachesSorted);

  assert(rendererCache->attributes > 0);
  assert(rendererCache->attributes <= vertexStream->attributes);

  // TODO:
  //  Used to reallocate whole geometry buffer, even if only one primitive render cache is
  //  added.
  //  Now buffer grows indefinitely. Implement some sort of garbage collection.
  // utarray_realloc(vertexStream->indexData, sizeof(uint32_t));
  // utarray_realloc(vertexStream->vertexData,
  //                vulkan_attribute_type_to_stride(vertexStream->attributes));

  // add unique primitives to geometry buffer
  vulkan_render_cache *lastRenderCache = NULL;
  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              rendererCache->primitiveRenderCaches) {
    // PERF: Compress stream (overlapping vertex attributes).

    if (lastRenderCache != NULL && lastRenderCache->primitive == renderCache->primitive) {
      size_t firstIndexOffset = lastRenderCache->firstIndexOffset;
      size_t firstVertexOffset = lastRenderCache->firstVertexOffset;
      vulkan_render_cache_set_vertex_stream_offsets(renderCache, firstIndexOffset,
                                                    firstVertexOffset);
      return;
    }

    vulkan_asset_primitive *primitive = renderCache->primitive;
    assert(primitive->indices->componentType == vulkan_asset_vertex_attribute_component_uint32_t);

    vulkan_vertex_stream_element vertexStreamElement = vulkan_vertex_stream_add_geometry(
        vertexStream, primitive->vertexCount, primitive->indices->data, primitive->positions->data,
        primitive->normals->data, primitive->colors->data, primitive->texCoords->data);

    assert(vertexStreamElement.attributes <= vertexStream->attributes);
    vulkan_render_cache_set_vertex_stream_offsets(renderCache, vertexStreamElement.firstIndexOffset,
                                                  vertexStreamElement.firstVertexOffset);

    lastRenderCache = renderCache;
  }

  rendererCache->primitiveRenderCachesDirty = false;
}

void vulkan_renderer_cache_update_textures(vulkan_renderer_cache *rendererCache,
                                           vulkan_textures *textures) {
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              rendererCache->primitiveRenderCaches) {
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
  // HIRO refactor move cache_list_sort_func tp batch.h
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

void vulkan_renderer_cache_sort_primitive_render_caches(vulkan_renderer_cache *rendererCache) {
  // HIRO refactor move vulkan_renderer_cache_sort_primitive_render_caches tp batch.h
  if (rendererCache->primitiveRenderCachesSorted) {
    return;
  }
  log_debug("sorting primitive render caches");

  // sort primitive render caches for batching
  utarray_sort(rendererCache->primitiveRenderCaches, cache_list_sort_func);

  rendererCache->primitiveRenderCachesSorted = true;
}

void vulkan_renderer_cache_calculate_aabb_for_primitive_render_caches(
    vulkan_renderer_cache *rendererCache) {
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              rendererCache->primitiveRenderCaches) {
    vulkan_aabb_add_aabb(&rendererCache->aabb, &primitiveRenderCache->aabb);
  }
}

void vulkan_renderer_cache_add_camera_render_cache(vulkan_renderer_cache *rendererCache,
                                                   vulkan_render_cache *cameraRenderCache) {
  utarray_push_back(rendererCache->cameraRenderCaches, &cameraRenderCache);
}

void vulkan_renderer_cache_add_other_render_cache(vulkan_renderer_cache *rendererCache,
                                                  vulkan_render_cache *otherRenderCache) {
  assert(otherRenderCache->primitive != NULL);

  utarray_push_back(rendererCache->otherRenderCaches, &otherRenderCache);

  rendererCache->otherRenderCachesDirty = true;
}

void vulkan_renderer_cache_add_skybox(vulkan_renderer_cache *rendererCache,
                                      vulkan_asset_skybox *skybox) {
  rendererCache->skybox = skybox;
}

void vulkan_renderer_cache_debug_print(vulkan_renderer_cache *rendererCache) {
  log_raw(stdout, "digraph primitive_renderer_cache {");
  utarray_foreach_elem_deref (vulkan_render_cache *, primitiveRenderCache,
                              rendererCache->primitiveRenderCaches) {
    vulkan_render_cache_debug_print(primitiveRenderCache);
  }
  log_raw(stdout, "}\n");

  log_raw(stdout, "digraph camera_renderer_cache {");
  utarray_foreach_elem_deref (vulkan_render_cache *, cameraRenderCache,
                              rendererCache->cameraRenderCaches) {
    vulkan_render_cache_debug_print(cameraRenderCache);
  }
  log_raw(stdout, "}\n");

  vulkan_aabb_debug_print(&rendererCache->aabb, 2);
}

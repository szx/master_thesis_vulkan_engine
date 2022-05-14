#include "renderer_cache.h"
#include "../assets/primitive.h"

vulkan_renderer_cache *vulkan_renderer_cache_create(size_t maxPrimitiveElementCount) {
  vulkan_renderer_cache *rendererCache = core_alloc(sizeof(vulkan_renderer_cache));

  rendererCache->maxPrimitiveElementCount = maxPrimitiveElementCount;
  rendererCache->primitiveElements = NULL;
  rendererCache->attributes = vulkan_attribute_type_unknown;
  rendererCache->aabb = vulkan_aabb_default();

  rendererCache->primitiveElements = NULL;
  rendererCache->cameraElements = NULL;

  // HIRO refactor default camera from scene data
  rendererCache->defaultCamera = core_alloc(sizeof(vulkan_asset_camera));
  vulkan_asset_camera_init(rendererCache->defaultCamera, NULL);
  rendererCache->skybox = NULL;

  rendererCache->_primitiveElementsDirty = false;
  utarray_alloc(rendererCache->_newPrimitiveElements,
                sizeof(vulkan_renderer_cache_primitive_element *));

  return rendererCache;
}

void vulkan_renderer_cache_destroy(vulkan_renderer_cache *rendererCache) {
  vulkan_asset_camera_deinit(rendererCache->defaultCamera);
  core_free(rendererCache->defaultCamera);

  dl_foreach_elem(vulkan_renderer_cache_camera_element *, element, rendererCache->cameraElements) {
    vulkan_renderer_cache_camera_element_destroy(element);
  }

  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, element,
                  rendererCache->primitiveElements) {
    vulkan_renderer_cache_primitive_element_destroy(element);
  }

  utarray_free(rendererCache->_newPrimitiveElements);
  core_free(rendererCache);
}

void vulkan_renderer_cache_add_primitive_element(
    vulkan_renderer_cache *rendererCache,
    vulkan_renderer_cache_primitive_element *primitiveElement) {

  assert(primitiveElement != NULL);
  assert(primitiveElement->primitive != NULL);
  verify(dl_count(rendererCache->primitiveElements) < rendererCache->maxPrimitiveElementCount);

  DL_APPEND(rendererCache->primitiveElements, primitiveElement);
  rendererCache->_primitiveElementsDirty = true;
  utarray_push_back(rendererCache->_newPrimitiveElements, &primitiveElement);

  rendererCache->attributes |= primitiveElement->primitive->attributes;

  // NOTE: Primitive element's AABB are is calculated in
  // sort_primitive_elements.

  // NOTE: Primitive element's geometry is added in vulkan_renderer_cache_update_geometry.
  // NOTE: Primitive element's textures are added in vulkan_renderer_cache_update_textures.
}

static int sort_primitive_elements_before_update_geometry_cmp(const void *aPtr, const void *bPtr) {
  const vulkan_renderer_cache_primitive_element *a =
      (const vulkan_renderer_cache_primitive_element *)aPtr;
  const vulkan_renderer_cache_primitive_element *b =
      (const vulkan_renderer_cache_primitive_element *)bPtr;

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

static void sort_primitive_elements_before_update_geometry(vulkan_renderer_cache *rendererCache) {
  log_debug("sorting primitive elements before update geometry");

  // sort primitive elements
  DL_SORT(rendererCache->primitiveElements, sort_primitive_elements_before_update_geometry_cmp);
}

void vulkan_renderer_cache_update_geometry(vulkan_renderer_cache *rendererCache,
                                           vulkan_vertex_stream *vertexStream) {
  if (!rendererCache->_primitiveElementsDirty) {
    return;
  }
  log_debug("updating rendererCache -> vulkan_vertex_stream");

  // Sort primitive elements by geometry to minimize geometry buffer size.
  sort_primitive_elements_before_update_geometry(rendererCache);

  assert(rendererCache->attributes > 0);
  assert(rendererCache->attributes <= vertexStream->attributes);

  // TODO:
  //  Used to reallocate whole geometry buffer, even if only one primitive element is
  //  added.
  //  Now buffer grows indefinitely. Implement some sort of garbage collection.
  // utarray_realloc(vertexStream->indexData, sizeof(uint32_t));
  // utarray_realloc(vertexStream->vertexData,
  //                vulkan_attribute_type_to_stride(vertexStream->attributes));

  // Add unique primitives to geometry buffer.
  vulkan_renderer_cache_primitive_element *lastPrimitiveElement = NULL;
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    // PERF: Compress stream (overlapping vertex attributes).

    if (lastPrimitiveElement != NULL &&
        lastPrimitiveElement->primitive == primitiveElement->primitive) {
      // Reuse geometry if last primitive is the same one.
      primitiveElement->vertexStreamElement = lastPrimitiveElement->vertexStreamElement;
      return;
    }

    vulkan_asset_primitive *primitive = primitiveElement->primitive;
    assert(primitive->indices->componentType == vulkan_asset_vertex_attribute_component_uint32_t);

    vulkan_vertex_stream_element vertexStreamElement = vulkan_vertex_stream_add_geometry(
        vertexStream, primitive->vertexCount, primitive->indices->data, primitive->positions->data,
        primitive->normals->data, primitive->colors->data, primitive->texCoords->data);

    assert(vertexStreamElement.attributes <= vertexStream->attributes);
    primitiveElement->vertexStreamElement = vertexStreamElement;

    lastPrimitiveElement = primitiveElement;
  }

  rendererCache->_primitiveElementsDirty = false;
}

void vulkan_renderer_cache_update_textures(vulkan_renderer_cache *rendererCache,
                                           vulkan_textures *textures) {
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    assert(primitiveElement->primitive != NULL);
    vulkan_asset_material *material = primitiveElement->primitive->material;
    if (material == NULL) {
      continue;
    }
    vulkan_textures_material_element *element = vulkan_textures_add_material(textures, material);
    primitiveElement->materialElement = element;

    // TODO: Unload unneeded textures.
  }
}

void vulkan_renderer_cache_add_new_primitive_elements_to_batches(
    vulkan_renderer_cache *rendererCache, vulkan_batches *batches) {
  // TODO: Replace with more generic function that supports removal al primitive elements.
  if (utarray_len(rendererCache->_newPrimitiveElements) == 0) {
    return;
  }
  log_debug("updating render cache -> batches");
  utarray_foreach_elem_deref (vulkan_renderer_cache_primitive_element *, primitiveElement,
                              rendererCache->_newPrimitiveElements) {
    vulkan_batches_add_primitive_element(batches, primitiveElement);
  }
  utarray_clear(rendererCache->_newPrimitiveElements);
}

void vulkan_renderer_cache_calculate_aabb_for_primitive_elements(
    vulkan_renderer_cache *rendererCache) {
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    vulkan_aabb_add_aabb(&rendererCache->aabb, &primitiveElement->aabb);
  }
}

void vulkan_renderer_cache_add_camera_element(vulkan_renderer_cache *rendererCache,
                                              vulkan_renderer_cache_camera_element *cameraElement) {
  DL_APPEND(rendererCache->cameraElements, cameraElement);
}

void vulkan_renderer_cache_add_skybox(vulkan_renderer_cache *rendererCache,
                                      vulkan_asset_skybox *skybox) {
  rendererCache->skybox = skybox;
}

void vulkan_renderer_cache_debug_print(vulkan_renderer_cache *rendererCache) {
  log_raw(stdout, "digraph primitive_renderer_cache {");
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    vulkan_renderer_cache_primitive_element_debug_print(primitiveElement);
  }
  log_raw(stdout, "}\n");

  log_raw(stdout, "digraph camera_renderer_cache {");
  dl_foreach_elem(vulkan_renderer_cache_camera_element *, cameraElement,
                  rendererCache->cameraElements) {
    vulkan_renderer_cache_camera_element_debug_print(cameraElement);
  }
  log_raw(stdout, "}\n");

  vulkan_aabb_debug_print(&rendererCache->aabb, 2);
}

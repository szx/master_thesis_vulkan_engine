#include "renderer_cache.h"
#include "../assets/primitive.h"

void add_basic_primitive_elements(vulkan_renderer_cache *rendererCache) {
  vulkan_asset_primitive *boxBasicPrimitive = vulkan_asset_primitive_create_from_geometry(
      rendererCache->sceneData, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 36,
      (uint32_t[36]){0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
      (vec3[36]){// -X (left) side,
                 {-1, -1, 1},
                 {-1, 1, -1},
                 {-1, -1, -1},
                 {-1, 1, -1},
                 {-1, -1, 1},
                 {-1, 1, 1},
                 // +X (right) side
                 {1, -1, -1},
                 {1, 1, 1},
                 {1, -1, 1},
                 {1, 1, 1},
                 {1, -1, -1},
                 {1, 1, -1},
                 // -Y (bottom) side
                 {-1, -1, -1},
                 {1, -1, -1},
                 {-1, -1, 1},
                 {1, -1, -1},
                 {1, -1, 1},
                 {-1, -1, 1},
                 // +Y (top) side
                 {-1, 1, -1},
                 {1, 1, 1},
                 {1, 1, -1},
                 {1, 1, 1},
                 {-1, 1, -1},
                 {-1, 1, 1},
                 // -Z (back) side
                 {-1, 1, -1},
                 {1, -1, -1},
                 {-1, -1, -1},
                 {1, -1, -1},
                 {-1, 1, -1},
                 {1, 1, -1},
                 // +Z (front) side
                 {-1, -1, 1},
                 {1, 1, 1},
                 {-1, 1, 1},
                 {1, 1, 1},
                 {-1, -1, 1},
                 {1, -1, 1}},
      NULL, NULL, NULL, NULL);
  rendererCache->basicBoxPrimitiveElement = vulkan_renderer_cache_primitive_element_create(
      vulkan_renderer_cache_primitive_element_source_type_basic, true, GLM_MAT4_IDENTITY,
      boxBasicPrimitive, vulkan_aabb_default());
  vulkan_renderer_cache_add_primitive_element(rendererCache,
                                              rendererCache->basicBoxPrimitiveElement);

  vulkan_asset_primitive *basicFullscreenTrianglePrimitive =
      vulkan_asset_primitive_create_from_geometry(
          rendererCache->sceneData, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 3, (uint32_t[3]){0, 1, 2},
          // fullscreen triangle in Vulkan clip space
          (vec3[3]){
              {-1, -1, 0},
              {-1, 3, 0},
              {3, -1, 0},
          },
          NULL, NULL, NULL, NULL);
  rendererCache->basicFullscreenTrianglePrimitiveElement =
      vulkan_renderer_cache_primitive_element_create(
          vulkan_renderer_cache_primitive_element_source_type_basic, true, GLM_MAT4_IDENTITY,
          basicFullscreenTrianglePrimitive, vulkan_aabb_default());
  vulkan_renderer_cache_add_primitive_element(
      rendererCache, rendererCache->basicFullscreenTrianglePrimitiveElement);
}

vulkan_renderer_cache *vulkan_renderer_cache_create(vulkan_scene_data *sceneData,
                                                    size_t maxPrimitiveElementCount) {
  vulkan_renderer_cache *rendererCache = core_alloc(sizeof(vulkan_renderer_cache));

  rendererCache->sceneData = sceneData;

  rendererCache->maxPrimitiveElementCount = maxPrimitiveElementCount;
  rendererCache->primitiveElements = NULL;
  rendererCache->attributes = vulkan_attribute_type_unknown;
  rendererCache->aabb = vulkan_aabb_default();

  rendererCache->primitiveElements = NULL;
  rendererCache->cameraElements = NULL;

  rendererCache->defaultCameraElement = vulkan_renderer_cache_camera_element_create(
      &rendererCache->sceneData->defaultCamera, GLM_MAT4_IDENTITY);
  rendererCache->skyboxElement = NULL;
  rendererCache->fontElement = NULL;

  rendererCache->_primitiveElementsDirty = false;
  utarray_alloc(rendererCache->_newPrimitiveElements,
                sizeof(vulkan_renderer_cache_primitive_element *));

  add_basic_primitive_elements(rendererCache);

  return rendererCache;
}

void vulkan_renderer_cache_destroy(vulkan_renderer_cache *rendererCache) {

  dl_foreach_elem(vulkan_renderer_cache_camera_element *, element, rendererCache->cameraElements) {
    vulkan_renderer_cache_camera_element_destroy(element);
  }

  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, element,
                  rendererCache->primitiveElements) {
    vulkan_renderer_cache_primitive_element_destroy(element);
  }

  vulkan_renderer_cache_camera_element_destroy(rendererCache->defaultCameraElement);
  vulkan_renderer_cache_skybox_element_destroy(rendererCache->skyboxElement);
  vulkan_renderer_cache_font_element_destroy(rendererCache->fontElement);

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
      continue;
    }

    vulkan_asset_primitive *primitive = primitiveElement->primitive;
    assert(primitive->indices->componentType == vulkan_asset_vertex_attribute_component_uint32_t);
    vulkan_vertex_stream_element vertexStreamElement = vulkan_vertex_stream_add_geometry(
        vertexStream, primitive->vertexCount, primitive->indices->data, primitive->positions->data,
        primitive->normals->data, primitive->colors->data, primitive->texCoords->data,
        primitive->tangents->data);

    assert(vertexStreamElement.attributes <= vertexStream->attributes);
    vulkan_renderer_cache_primitive_set_vulkan_vertex_stream_element(primitiveElement,
                                                                     vertexStreamElement);

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
    vulkan_textures_material_element *materialElement =
        vulkan_textures_add_material(textures, material);
    vulkan_renderer_cache_primitive_set_material_element(primitiveElement, materialElement);

    // TODO: Unload unneeded textures.
  }

  rendererCache->skyboxElement->cubemapMaterialElement =
      vulkan_textures_add_material(textures, &rendererCache->skyboxElement->cubemapMaterial);

  rendererCache->fontElement->fontMaterialElement =
      vulkan_textures_add_material(textures, &rendererCache->fontElement->fontMaterial);
}

void vulkan_renderer_cache_add_new_primitive_elements_to_batches(
    vulkan_renderer_cache *rendererCache, vulkan_batches *batches,
    vulkan_renderer_cache_primitive_element_source_type sourceType) {
  if (utarray_len(rendererCache->_newPrimitiveElements) == 0) {
    return;
  }
  log_debug("updating render cache -> batches");
  utarray_foreach_elem_deref (vulkan_renderer_cache_primitive_element *, primitiveElement,
                              rendererCache->_newPrimitiveElements) {
    if (primitiveElement->sourceType == sourceType) {
      vulkan_batches_add_primitive_element(batches, primitiveElement);
    }
  }
  utarray_clear(rendererCache->_newPrimitiveElements);
}

void vulkan_renderer_cache_calculate_aabb_for_primitive_elements(
    vulkan_renderer_cache *rendererCache) {
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    if (primitiveElement->visible) {
      vulkan_aabb_add_aabb(&rendererCache->aabb, &primitiveElement->aabb);
    }
  }
}

void vulkan_renderer_cache_add_camera_element(vulkan_renderer_cache *rendererCache,
                                              vulkan_renderer_cache_camera_element *cameraElement) {
  DL_APPEND(rendererCache->cameraElements, cameraElement);
}

void vulkan_renderer_cache_add_skybox(vulkan_renderer_cache *rendererCache,
                                      vulkan_renderer_cache_skybox_element *skyboxElement) {
  rendererCache->skyboxElement = skyboxElement;
}

void vulkan_renderer_cache_add_font(vulkan_renderer_cache *rendererCache,
                                    vulkan_renderer_cache_font_element *fontElement) {
  rendererCache->fontElement = fontElement;
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

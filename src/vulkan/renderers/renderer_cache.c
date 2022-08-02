#include "renderer_cache.h"
#include "../assets/primitive.h"

void add_basic_primitive_elements(renderer_cache *rendererCache) {
  asset_primitive *boxBasicPrimitive = asset_primitive_create_from_geometry(
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
  rendererCache->basicBoxPrimitiveElement = renderer_cache_primitive_element_create(
      renderer_cache_primitive_element_source_type_basic, true, GLM_MAT4_IDENTITY,
      boxBasicPrimitive, aabb_default());
  renderer_cache_add_primitive_element(rendererCache, rendererCache->basicBoxPrimitiveElement);

  asset_primitive *basicFullscreenTrianglePrimitive = asset_primitive_create_from_geometry(
      rendererCache->sceneData, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 3, (uint32_t[3]){0, 1, 2},
      // fullscreen triangle in Vulkan clip space
      (vec3[3]){
          {-1, -1, 0},
          {-1, 3, 0},
          {3, -1, 0},
      },
      NULL, NULL, NULL, NULL);
  rendererCache->basicFullscreenTrianglePrimitiveElement = renderer_cache_primitive_element_create(
      renderer_cache_primitive_element_source_type_basic, true, GLM_MAT4_IDENTITY,
      basicFullscreenTrianglePrimitive, aabb_default());
  renderer_cache_add_primitive_element(rendererCache,
                                       rendererCache->basicFullscreenTrianglePrimitiveElement);
}

renderer_cache *renderer_cache_create(scene_data *sceneData, size_t maxPrimitiveElementCount) {
  renderer_cache *rendererCache = core_alloc(sizeof(renderer_cache));

  rendererCache->sceneData = sceneData;

  rendererCache->maxPrimitiveElementCount = maxPrimitiveElementCount;
  rendererCache->primitiveElements = NULL;
  rendererCache->attributes = vertex_attribute_type_unknown;
  rendererCache->aabb = aabb_default();

  rendererCache->primitiveElements = NULL;
  rendererCache->cameraElements = NULL;

  rendererCache->defaultCameraElement = renderer_cache_camera_element_create(
      &rendererCache->sceneData->defaultCamera, GLM_MAT4_IDENTITY);
  rendererCache->directLightElements = NULL;
  rendererCache->skyboxElement = NULL;
  rendererCache->fontElement = NULL;

  rendererCache->_primitiveElementsDirty = false;
  utarray_alloc(rendererCache->_newPrimitiveElements, sizeof(renderer_cache_primitive_element *));

  add_basic_primitive_elements(rendererCache);

  return rendererCache;
}

void renderer_cache_destroy(renderer_cache *rendererCache) {

  dl_foreach_elem(renderer_cache_camera_element *, element, rendererCache->cameraElements) {
    renderer_cache_camera_element_destroy(element);
  }

  dl_foreach_elem(renderer_cache_primitive_element *, element, rendererCache->primitiveElements) {
    renderer_cache_primitive_element_destroy(element);
  }

  renderer_cache_camera_element_destroy(rendererCache->defaultCameraElement);

  dl_foreach_elem(renderer_cache_direct_light_element *, element,
                  rendererCache->directLightElements) {
    renderer_cache_direct_light_element_destroy(element);
  }

  renderer_cache_skybox_element_destroy(rendererCache->skyboxElement);
  renderer_cache_font_element_destroy(rendererCache->fontElement);

  utarray_free(rendererCache->_newPrimitiveElements);
  core_free(rendererCache);
}

void renderer_cache_add_primitive_element(renderer_cache *rendererCache,
                                          renderer_cache_primitive_element *primitiveElement) {

  assert(primitiveElement != NULL);
  assert(primitiveElement->primitive != NULL);
  assert(utarray_len(primitiveElement->primitive->indices->data) > 0);
  verify(dl_count(rendererCache->primitiveElements) < rendererCache->maxPrimitiveElementCount);

  DL_APPEND(rendererCache->primitiveElements, primitiveElement);
  rendererCache->_primitiveElementsDirty = true;
  utarray_push_back(rendererCache->_newPrimitiveElements, &primitiveElement);

  rendererCache->attributes |= primitiveElement->primitive->attributes;

  // NOTE: Primitive element's AABB are is calculated in
  // sort_primitive_elements.

  // NOTE: Primitive element's geometry is added in renderer_cache_update_geometry.
  // NOTE: Primitive element's textures are added in renderer_cache_update_textures.
}

static int sort_primitive_elements_before_update_geometry_cmp(const void *aPtr, const void *bPtr) {
  const renderer_cache_primitive_element *a = (const renderer_cache_primitive_element *)aPtr;
  const renderer_cache_primitive_element *b = (const renderer_cache_primitive_element *)bPtr;

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

static void sort_primitive_elements_before_update_geometry(renderer_cache *rendererCache) {
  log_debug("sorting primitive elements before update geometry");

  // sort primitive elements
  DL_SORT(rendererCache->primitiveElements, sort_primitive_elements_before_update_geometry_cmp);
}

void renderer_cache_update_geometry(renderer_cache *rendererCache, vertex_stream *vertexStream) {
  if (!rendererCache->_primitiveElementsDirty) {
    return;
  }
  log_debug("updating rendererCache -> vertex_stream");

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
  //                vertex_attribute_type_to_stride(vertexStream->attributes));

  // Add unique primitives to geometry buffer.
  renderer_cache_primitive_element *lastPrimitiveElement = NULL;
  dl_foreach_elem(renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    // PERF: Compress stream (overlapping vertex attributes).

    if (lastPrimitiveElement != NULL &&
        lastPrimitiveElement->primitive == primitiveElement->primitive) {
      // Reuse geometry if last primitive is the same one.
      primitiveElement->vertexStreamElement = lastPrimitiveElement->vertexStreamElement;
      continue;
    }

    asset_primitive *primitive = primitiveElement->primitive;
    assert(primitive->indices->componentType == asset_vertex_attribute_component_uint32_t);
    vertex_stream_element vertexStreamElement = vertex_stream_add_geometry(
        vertexStream, primitive->vertexCount, primitive->indices->data, primitive->positions->data,
        primitive->normals->data, primitive->colors->data, primitive->texCoords->data,
        primitive->tangents->data);

    assert(vertexStreamElement.attributes <= vertexStream->attributes);
    renderer_cache_primitive_set_vertex_stream_element(primitiveElement, vertexStreamElement);

    lastPrimitiveElement = primitiveElement;
  }

  rendererCache->_primitiveElementsDirty = false;
}

void renderer_cache_update_textures(renderer_cache *rendererCache, textures *textures) {
  dl_foreach_elem(renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    assert(primitiveElement->primitive != NULL);
    asset_material *material = primitiveElement->primitive->material;
    if (material == NULL) {
      continue;
    }
    textures_material_element *materialElement = textures_add_material(textures, material);
    renderer_cache_primitive_set_material_element(primitiveElement, materialElement);

    // TODO: Unload unneeded textures.
  }

  rendererCache->skyboxElement->skyboxTextureElement = textures_add_texture(
      textures, rendererCache->skyboxElement->skybox->cubemapTexture, "skybox");

  rendererCache->fontElement->fontTextureElement =
      textures_add_texture(textures, rendererCache->fontElement->font->fontTexture, "font");
}

void renderer_cache_add_new_primitive_elements_to_batches(
    renderer_cache *rendererCache, batches *batches,
    renderer_cache_primitive_element_source_type sourceType) {
  if (utarray_len(rendererCache->_newPrimitiveElements) == 0) {
    return;
  }
  log_debug("updating render cache -> batches");
  utarray_foreach_elem_deref (renderer_cache_primitive_element *, primitiveElement,
                              rendererCache->_newPrimitiveElements) {
    if (primitiveElement->sourceType == sourceType) {
      batches_add_primitive_element(batches, primitiveElement);
    }
  }
  utarray_clear(rendererCache->_newPrimitiveElements);
}

void renderer_cache_calculate_aabb_for_primitive_elements(renderer_cache *rendererCache) {
  dl_foreach_elem(renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    if (primitiveElement->visible) {
      aabb_add_aabb(&rendererCache->aabb, &primitiveElement->aabb);
    }
  }
}

void renderer_cache_add_camera_element(renderer_cache *rendererCache,
                                       renderer_cache_camera_element *cameraElement) {
  DL_APPEND(rendererCache->cameraElements, cameraElement);
}

void renderer_cache_add_direct_light_element(
    renderer_cache *rendererCache, renderer_cache_direct_light_element *directLightElement) {
  DL_APPEND(rendererCache->directLightElements, directLightElement);
}

void renderer_cache_remove_direct_light_element(
    renderer_cache *rendererCache, renderer_cache_direct_light_element *directLightElement) {
  DL_DELETE(rendererCache->directLightElements, directLightElement);
}

void renderer_cache_add_skybox(renderer_cache *rendererCache,
                               renderer_cache_skybox_element *skyboxElement) {
  rendererCache->skyboxElement = skyboxElement;
}

void renderer_cache_add_font(renderer_cache *rendererCache,
                             renderer_cache_font_element *fontElement) {
  rendererCache->fontElement = fontElement;
}

void renderer_cache_debug_print(renderer_cache *rendererCache) {
  log_raw(stdout, "digraph primitive_renderer_cache {");
  dl_foreach_elem(renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    renderer_cache_primitive_element_debug_print(primitiveElement);
  }
  log_raw(stdout, "}\n");

  log_raw(stdout, "digraph camera_renderer_cache {");
  dl_foreach_elem(renderer_cache_camera_element *, cameraElement, rendererCache->cameraElements) {
    renderer_cache_camera_element_debug_print(cameraElement);
  }
  log_raw(stdout, "}\n");

  aabb_debug_print(&rendererCache->aabb, 2);
}

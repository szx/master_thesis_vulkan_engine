/* Vulkan renderer cache elements.
 * Describe renderer cache.
 */
#pragma once

#include "../assets/assets.h"
#include "../common.h"
#include "../objects/textures.h"
#include "../objects/vertex_stream.h"

// TODO: Add key to prevent multiple adding in vulkan_renderer_cache?

/* primitive renderer cache element */

typedef struct vulkan_renderer_cache_primitive_element {
  /* cache state set by scene tree (or manually) */
  bool visible;
  mat4 transform;                    ///< Accumulated from object node.
  vulkan_asset_primitive *primitive; ///< Accumulated from primitive node.
  vulkan_aabb aabb;                  ///< Accumulated from primitive node.

  /* cache state set by vertex stream */
  vulkan_vertex_stream_element vertexStreamElement;

  /* cache state set during draw call batching. */
  size_t instanceId; ///< Index in renderer cache, equals gl_InstanceIndex in shader thanks to
                     ///< draw call batching.

  /* cache state set during unified uniform buffer update */
  vulkan_textures_material_element
      *materialElement; ///< Contains index of material in materials array in textures.

  struct vulkan_renderer_cache_primitive_element *prev, *next;

  bool _ownsPrimitive;
} vulkan_renderer_cache_primitive_element;

vulkan_renderer_cache_primitive_element *
vulkan_renderer_cache_primitive_element_create(bool visible, mat4 transform,
                                               vulkan_asset_primitive *primitive, vulkan_aabb aabb);

vulkan_renderer_cache_primitive_element *
vulkan_renderer_cache_primitive_element_create_from_geometry(bool visible, mat4 transform,
                                                             uint32_t vertexCount,
                                                             uint32_t *indices, vec3 *positions,
                                                             vec3 *normals, vec3 *colors,
                                                             vec2 *texCoords);

void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element);

void vulkan_renderer_cache_primitive_element_debug_print(
    vulkan_renderer_cache_primitive_element *element);

/* camera renderer cache element */

typedef struct vulkan_renderer_cache_camera_element {
  /* cache state accumulated from scene tree */
  mat4 transform;              ///< Accumulated from object node.
  vulkan_asset_camera *camera; ///< Accumulated from object node.

  struct vulkan_renderer_cache_camera_element *prev, *next;
} vulkan_renderer_cache_camera_element;

vulkan_renderer_cache_camera_element *
vulkan_renderer_cache_camera_element_create(vulkan_asset_camera *camera, mat4 transform);

void vulkan_renderer_cache_camera_element_destroy(vulkan_renderer_cache_camera_element *element);

void vulkan_renderer_cache_camera_element_debug_print(
    vulkan_renderer_cache_camera_element *element);

/* skybox renderer cache element */

typedef struct vulkan_renderer_cache_skybox_element {
  /* cache state accumulated from scene tree */
  vulkan_asset_skybox *skybox; ///< Accumulated from scene data.
  vulkan_asset_material cubemapMaterial;

  /* cache state set during texture update */
  vulkan_textures_material_element *cubemapMaterialElement;

  struct vulkan_renderer_cache_camera_element *prev, *next;
} vulkan_renderer_cache_skybox_element;

vulkan_renderer_cache_skybox_element *
vulkan_renderer_cache_skybox_element_create(vulkan_asset_skybox *skybox);

void vulkan_renderer_cache_skybox_element_destroy(vulkan_renderer_cache_skybox_element *element);

void vulkan_renderer_cache_skybox_element_debug_print(
    vulkan_renderer_cache_skybox_element *element);

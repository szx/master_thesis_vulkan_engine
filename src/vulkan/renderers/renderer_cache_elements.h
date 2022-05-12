/* Vulkan renderer cache elements.
 * Describe renderer cache.
 */
#pragma once

#include "../assets/assets.h"
#include "../common.h"
#include "../objects/textures.h"
#include "../objects/vertex_stream.h"

// HIRO add key to prevent multiple adding in vulkan_renderer_cache

/* primitive renderer cache element */

typedef struct vulkan_renderer_cache_primitive_element {
  /* cache state accumulated from scene tree */
  size_t distanceFromRoot;
  bool visible;
  mat4 transform;                    ///< Accumulated from object node.
  vulkan_asset_mesh *mesh;           ///< Accumulated from object node.
  vulkan_asset_primitive *primitive; ///< Accumulated from primitive node.
  vulkan_aabb aabb;                  ///< Accumulated from primitive node.

  /* cache state accumulated from vertex stream */
  vulkan_vertex_stream_element vertexStreamElement;

  /* cache state accumulated during draw call batching. */
  size_t instanceId; ///< Index in renderer cache, equals gl_InstanceIndex in shader thanks to
                     ///< draw call batching.

  /* cache state accumulated from unified uniform buffer update */
  vulkan_textures_material_element
      *materialElement; ///< Contains index of material in materials array in textures.

  struct vulkan_renderer_cache_primitive_element *prev, *next;
} vulkan_renderer_cache_primitive_element;

vulkan_renderer_cache_primitive_element *vulkan_renderer_cache_primitive_element_create();
void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element);

void vulkan_renderer_cache_primitive_element_reset(
    vulkan_renderer_cache_primitive_element *element);

void vulkan_renderer_cache_primitive_element_debug_print(
    vulkan_renderer_cache_primitive_element *element);

/* camera renderer cache element */

typedef struct vulkan_renderer_cache_camera_element {
  /* cache state accumulated from scene tree */
  mat4 transform;             ///< Accumulated from object node.
  vulkan_asset_camera camera; ///< Accumulated from object node.

  struct vulkan_renderer_cache_camera_element *prev, *next;
} vulkan_renderer_cache_camera_element;

vulkan_renderer_cache_camera_element *vulkan_renderer_cache_camera_element_create();
void vulkan_renderer_cache_camera_element_destroy(vulkan_renderer_cache_camera_element *element);

void vulkan_renderer_cache_camera_element_debug_print(
    vulkan_renderer_cache_camera_element *element);

// HIRO refactor renderer cache skybox element

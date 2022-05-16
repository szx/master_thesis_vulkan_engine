/* Vulkan renderer cache elements.
 * Describe renderer cache.
 */
#pragma once

#include "../assets/assets.h"
#include "../common.h"
#include "../objects/textures.h"
#include "../objects/vertex_stream.h"

/* primitive renderer cache element */

/// Describes which object has added a particular renderer cache primitive element.
typedef enum vulkan_renderer_cache_primitive_element_source_type {
  vulkan_renderer_cache_primitive_element_source_type_scene_tree,
  vulkan_renderer_cache_primitive_element_source_type_basic,
  vulkan_renderer_cache_primitive_element_source_type_count,
} vulkan_renderer_cache_primitive_element_source_type;

typedef struct vulkan_renderer_cache_primitive_element {
  vulkan_renderer_cache_primitive_element_source_type sourceType;

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

#if defined(DEBUG)
  /// Prevents using the primitive element in multiple batches (which could result in multiple
  /// instanceIds).
  bool _instanceIdSet;
  /// Prevents recording draw command for unset vertexStreamElement;
  bool _vertexStreamElementSet;
  /// Prevents recording draw command for unset materialElement;
  bool _materialElementSet;
#endif

  struct vulkan_renderer_cache_primitive_element *prev, *next;

} vulkan_renderer_cache_primitive_element;

vulkan_renderer_cache_primitive_element *vulkan_renderer_cache_primitive_element_create(
    vulkan_renderer_cache_primitive_element_source_type sourceType, bool visible, mat4 transform,
    vulkan_asset_primitive *primitive, vulkan_aabb aabb);

void vulkan_renderer_cache_primitive_element_destroy(
    vulkan_renderer_cache_primitive_element *element);

void vulkan_renderer_cache_primitive_set_vulkan_vertex_stream_element(
    vulkan_renderer_cache_primitive_element *element,
    vulkan_vertex_stream_element vertexStreamElement);

void vulkan_renderer_cache_primitive_set_instance_id(
    vulkan_renderer_cache_primitive_element *element, size_t instanceId);

void vulkan_renderer_cache_primitive_set_material_element(
    vulkan_renderer_cache_primitive_element *element,
    vulkan_textures_material_element *materialElement);

#if defined(DEBUG)
#define vulkan_renderer_cache_primitive_is_valid(_element, _field) (_element)->_##_field##Set
#else
#define vulkan_renderer_cache_primitive_is_valid(_element, _field) true
#endif

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

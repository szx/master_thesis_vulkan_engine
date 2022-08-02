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
typedef enum renderer_cache_primitive_element_source_type {
  renderer_cache_primitive_element_source_type_scene_tree,
  renderer_cache_primitive_element_source_type_basic,
  renderer_cache_primitive_element_source_type_count,
} renderer_cache_primitive_element_source_type;

typedef struct renderer_cache_primitive_element {
  renderer_cache_primitive_element_source_type sourceType;

  /* cache state set by scene tree (or manually) */
  bool visible;
  mat4 transform;             ///< Accumulated from object node.
  asset_primitive *primitive; ///< Accumulated from primitive node.
  aabb aabb;                  ///< Accumulated from primitive node.

  /* cache state set by vertex stream */
  vertex_stream_element vertexStreamElement;

  /* cache state set during draw call batching. */
  size_t instanceId; ///< Index in renderer cache, equals gl_InstanceIndex in shader thanks to
                     ///< draw call batching.

  /* cache state set during unified uniform buffer update */
  textures_material_element
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

  struct renderer_cache_primitive_element *prev, *next;

} renderer_cache_primitive_element;

renderer_cache_primitive_element *
renderer_cache_primitive_element_create(renderer_cache_primitive_element_source_type sourceType,
                                        bool visible, mat4 transform, asset_primitive *primitive,
                                        aabb aabb);

void renderer_cache_primitive_element_destroy(renderer_cache_primitive_element *element);

void renderer_cache_primitive_set_vertex_stream_element(renderer_cache_primitive_element *element,
                                                        vertex_stream_element vertexStreamElement);

void renderer_cache_primitive_set_instance_id(renderer_cache_primitive_element *element,
                                              size_t instanceId);

void renderer_cache_primitive_set_material_element(renderer_cache_primitive_element *element,
                                                   textures_material_element *materialElement);

#if defined(DEBUG)
#define renderer_cache_primitive_is_valid(_element, _field) (_element)->_##_field##Set
#else
#define renderer_cache_primitive_is_valid(_element, _field) true
#endif

void renderer_cache_primitive_element_debug_print(renderer_cache_primitive_element *element);

/* camera renderer cache element */

typedef struct renderer_cache_camera_element {
  /* cache state accumulated from scene tree */
  mat4 transform;       ///< Accumulated from object node.
  asset_camera *camera; ///< Accumulated from object node.

  struct renderer_cache_camera_element *prev, *next;
} renderer_cache_camera_element;

renderer_cache_camera_element *renderer_cache_camera_element_create(asset_camera *camera,
                                                                    mat4 transform);

void renderer_cache_camera_element_destroy(renderer_cache_camera_element *element);

void renderer_cache_camera_element_debug_print(renderer_cache_camera_element *element);

/* direct light renderer cache element */

typedef struct renderer_cache_direct_light_element {
  /* cache state accumulated from scene data */
  asset_direct_light *directLight; ///< Accumulated from scene data.

  struct renderer_cache_direct_light_element *prev, *next;
} renderer_cache_direct_light_element;

renderer_cache_direct_light_element *
renderer_cache_direct_light_element_create(asset_direct_light *directLight);

void renderer_cache_direct_light_element_destroy(renderer_cache_direct_light_element *element);

void renderer_cache_direct_light_element_debug_print(renderer_cache_direct_light_element *element);

/* skybox renderer cache element */

typedef struct renderer_cache_skybox_element {
  /* cache state accumulated from scene tree */
  asset_skybox *skybox;

  /* cache state set during texture update */
  textures_texture_element *skyboxTextureElement;

} renderer_cache_skybox_element;

renderer_cache_skybox_element *renderer_cache_skybox_element_create(asset_skybox *skybox);

void renderer_cache_skybox_element_destroy(renderer_cache_skybox_element *element);

void renderer_cache_skybox_element_debug_print(renderer_cache_skybox_element *element);

/* font renderer cache element */

typedef struct renderer_cache_font_element {
  /* cache state accumulated from scene tree */
  asset_font *font; ///< Accumulated from scene data.

  /* cache state set during texture update */
  textures_texture_element *fontTextureElement;

} renderer_cache_font_element;

renderer_cache_font_element *renderer_cache_font_element_create(asset_font *font);

void renderer_cache_font_element_destroy(renderer_cache_font_element *element);

void renderer_cache_font_element_debug_print(renderer_cache_font_element *element);

/* Vulkan API helper objects. */

#pragma once

#include "../codegen/constants.h"
#include "../core/core.h"
#include "../data/data.h"
#include "debug.h"
#include "objects/device.h"

typedef enum vertex_attribute_type {
  vertex_attribute_type_unknown = 0,
  vertex_attribute_type_position = 1 << 0,
  vertex_attribute_type_normal = 1 << 1,
  vertex_attribute_type_color = 1 << 2,
  vertex_attribute_type_texcoord = 1 << 3,
  vertex_attribute_type_tangent = 1 << 4,
} vertex_attribute_type;

typedef enum image_type {
  image_type_undefined,
  image_type_swap_chain,
  image_type_material_base_color,
  image_type_material_parameters,
  image_type_material_normal_map,
  image_type_cubemap,
  image_type_font_bitmap,
  image_type_offscreen_first,
  image_type_offscreen_f16 = image_type_offscreen_first,
  image_type_offscreen_depth_buffer,
  image_type_offscreen_r8,
  image_type_offscreen_count,
  image_type_count
} image_type;

typedef enum image_render_pass_usage {
  image_render_pass_usage_read = (1 << 0),
  image_render_pass_usage_write = (1 << 1),
  image_render_pass_usage_read_write = image_render_pass_usage_read | image_render_pass_usage_write,
} image_render_pass_usage;

typedef struct image_type_info {
  bool depthFormat;
  bool colorFormat;

  bool uNormEncoding;
  bool sRGBEncoding;
  bool floatingPointEncoding;

  char componentBits;
  uint32_t forcedChannels;
} image_type_info;

extern image_type_info vulkanImageTypeInfo[];

typedef enum camera_type {
  camera_type_orthographic,
  camera_type_perspective,
  camera_type_count
} camera_type;

typedef enum direct_light_type {
  direct_light_type_directional,
  direct_light_type_point,
  direct_light_type_count,
} direct_light_type;

/// Axis-aligned bounding box.
typedef struct aabb {
  vec4 min;
  vec4 max;
} aabb;

aabb aabb_default();
void aabb_add_vec3(aabb *aabb, vec3 other);
void aabb_add_aabb(aabb *aabb, struct aabb *other);
void aabb_mul_mat4(aabb *aabb, mat4 transform);
void aabb_debug_print(aabb *aabb, int indent);

/// Calculates perspective projection matrix for left handed world/view-space to right-handed
/// flipped Vulkan clip space.
void get_perspective_matrix(float fovy, float aspect, float nearZ, float farZ, mat4 dest);

/// Calculates orthographic projection matrix for left handed world/view-space to right-handed
/// flipped Vulkan clip space.
void get_orthographic_matrix(float left, float right, float bottom, float top, float nearZ,
                             float farZ, mat4 dest);

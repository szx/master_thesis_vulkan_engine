#include "common.h"

// clang-format off
image_type_info vulkanImageTypeInfo[] = {
    [image_type_material_base_color] = {.colorFormat = true, .sRGBEncoding = true, .componentBits=8},
    [image_type_material_parameters] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [image_type_material_normal_map] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [image_type_cubemap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [image_type_font_bitmap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [image_type_offscreen_f16] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16, .forcedChannels = 4},
    [image_type_offscreen_depth_buffer] = {.depthFormat= true, .forcedChannels = 1},
    [image_type_offscreen_r8] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8, .forcedChannels = 1},
};
// clang-format on

aabb aabb_default() { return (aabb){{FLT_MAX, FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX, -FLT_MAX}}; }

void aabb_add_vec3(aabb *aabb, vec3 other) {
  aabb->min[0] = glm_min(aabb->min[0], other[0]);
  aabb->min[1] = glm_min(aabb->min[1], other[1]);
  aabb->min[2] = glm_min(aabb->min[2], other[2]);
  aabb->min[3] = 1.0f; // Homogenous coordinates.
  aabb->max[0] = glm_max(aabb->max[0], other[0]);
  aabb->max[1] = glm_max(aabb->max[1], other[1]);
  aabb->max[2] = glm_max(aabb->max[2], other[2]);
  aabb->max[3] = 1.0f; // Homogenous coordinates.
}

void aabb_add_aabb(aabb *aabb, struct aabb *other) {
  aabb->min[0] = glm_min(aabb->min[0], other->min[0]);
  aabb->min[1] = glm_min(aabb->min[1], other->min[1]);
  aabb->min[2] = glm_min(aabb->min[2], other->min[2]);
  aabb->min[3] = glm_min(aabb->min[3], other->min[3]);
  aabb->max[0] = glm_max(aabb->max[0], other->max[0]);
  aabb->max[1] = glm_max(aabb->max[1], other->max[1]);
  aabb->max[2] = glm_max(aabb->max[2], other->max[2]);
  aabb->max[3] = glm_max(aabb->max[3], other->max[3]);
}

void aabb_mul_mat4(aabb *aabb, mat4 transform) {
  glm_mat4_mulv(transform, aabb->min, aabb->min);
  glm_mat4_mulv(transform, aabb->max, aabb->max);
}

void aabb_debug_print(aabb *aabb, int indent) {
  log_debug(INDENT_FORMAT_STRING "aabb:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "min=(%f,%f,%f,%f)", INDENT_FORMAT_ARGS(2), aabb->min[0],
            aabb->min[1], aabb->min[2], aabb->min[3]);
  log_debug(INDENT_FORMAT_STRING "max=(%f,%f,%f,%f)", INDENT_FORMAT_ARGS(2), aabb->max[0],
            aabb->max[1], aabb->max[2], aabb->max[3]);
}

void get_perspective_matrix(float fovy, float aspect, float nearZ, float farZ, mat4 dest) {
  glm_mat4_zero(dest);

  // https://vincent-p.github.io/posts/perspective_matrix/
  float f;
  f = 1.0f / tanf(fovy * 0.5f);
  float x = f / aspect;
  float y = -f;
  float A = nearZ / (farZ - nearZ);
  float B = (nearZ * farZ) / (farZ - nearZ);

  dest[0][0] = x;
  dest[1][1] = y;
  dest[2][2] = A;
  dest[2][3] = -1.0f;
  dest[3][2] = B;
}

void get_orthographic_matrix(float left, float right, float bottom, float top, float nearZ,
                             float farZ, mat4 dest) {
  glm_mat4_zero(dest);

  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#orthographic-projection
  // https://dev.theomader.com/depth-precision/
  dest[0][0] = 1.0 / (right - left);
  dest[1][1] = 1.0 / (top - bottom);
  dest[2][2] = 2.0f / (nearZ - farZ);
  dest[3][2] = (farZ + nearZ) / (nearZ - farZ);
  dest[3][3] = 1.0f;

  // Flip Y:
  dest[1][1] = -dest[1][1];

  // Reverse Z:
  dest[2][2] = -dest[2][2];
  dest[3][2] = -dest[3][2];
}

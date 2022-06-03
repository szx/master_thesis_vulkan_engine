#include "common.h"

// clang-format off
vulkan_image_type_info vulkanImageTypeInfo[] = {
    [vulkan_image_type_material_base_color] = {.colorFormat = true, .sRGBEncoding = true, .componentBits=8},
    [vulkan_image_type_material_parameters] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_material_normal_map] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_cubemap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_font_bitmap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_offscreen_f16] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16, .forcedChannels = 4},
    [vulkan_image_type_offscreen_depth_buffer] = {.depthFormat= true, .forcedChannels = 1},
};
// clang-format on

vulkan_aabb vulkan_aabb_default() {
  return (vulkan_aabb){{FLT_MAX, FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX, -FLT_MAX}};
}

void vulkan_aabb_add_vec3(vulkan_aabb *aabb, vec3 other) {
  aabb->min[0] = MIN(aabb->min[0], other[0]);
  aabb->min[1] = MIN(aabb->min[1], other[1]);
  aabb->min[2] = MIN(aabb->min[2], other[2]);
  aabb->min[3] = 1.0f; // Homogenous coordinates.
  aabb->max[0] = MAX(aabb->max[0], other[0]);
  aabb->max[1] = MAX(aabb->max[1], other[1]);
  aabb->max[2] = MAX(aabb->max[2], other[2]);
  aabb->max[3] = 1.0f; // Homogenous coordinates.
}

void vulkan_aabb_add_aabb(vulkan_aabb *aabb, vulkan_aabb *other) {
  aabb->min[0] = MIN(aabb->min[0], other->min[0]);
  aabb->min[1] = MIN(aabb->min[1], other->min[1]);
  aabb->min[2] = MIN(aabb->min[2], other->min[2]);
  aabb->min[3] = MIN(aabb->min[3], other->min[3]);
  aabb->max[0] = MAX(aabb->max[0], other->max[0]);
  aabb->max[1] = MAX(aabb->max[1], other->max[1]);
  aabb->max[2] = MAX(aabb->max[2], other->max[2]);
  aabb->max[3] = MAX(aabb->max[3], other->max[3]);
}

void vulkan_aabb_mul_mat4(vulkan_aabb *aabb, mat4 transform) {
  glm_mat4_mulv(transform, aabb->min, aabb->min);
  glm_mat4_mulv(transform, aabb->max, aabb->max);
}

void vulkan_aabb_debug_print(vulkan_aabb *aabb, int indent) {
  log_debug(INDENT_FORMAT_STRING "aabb:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "min=(%f,%f,%f,%f)", INDENT_FORMAT_ARGS(2), aabb->min[0],
            aabb->min[1], aabb->min[2], aabb->min[3]);
  log_debug(INDENT_FORMAT_STRING "max=(%f,%f,%f,%f)", INDENT_FORMAT_ARGS(2), aabb->max[0],
            aabb->max[1], aabb->max[2], aabb->max[3]);
}

void vulkan_get_perspective_matrix(float fovy, float aspect, float nearZ, float farZ, mat4 dest) {
  glm_mat4_zero(dest);

  // https://vincent-p.github.io/posts/vulkan_perspective_matrix/
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

void vulkan_get_orthographic_matrix(float left, float right, float bottom, float top, float nearZ,
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

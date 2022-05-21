#include "common.h"

// HIRO gBuffer config from #include "../codegen/descriptors.h"

// clang-format off
vulkan_image_type_info vulkanImageTypeInfo[] = {
    [vulkan_image_type_depth_buffer] = {.depthFormat = true,},
    [vulkan_image_type_material_base_color] = {.colorFormat = true, .sRGBEncoding = true, .componentBits=8},
    [vulkan_image_type_material_parameters] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_cubemap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_font_bitmap] = {.colorFormat = true, .uNormEncoding = true, .componentBits=8},
    [vulkan_image_type_g_buffer_0] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16},
    [vulkan_image_type_g_buffer_1] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16},
    [vulkan_image_type_g_buffer_2] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16},
    [vulkan_image_type_g_buffer_3] = {.colorFormat = true, .floatingPointEncoding = true, .componentBits=16},
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

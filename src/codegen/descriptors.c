/* This file was autogenerated. */

#include "descriptors.h"
#include "../core/core.h"

void glsl_add_vulkan_draw_push_constant(UT_string *s) {
  utstring_printf(s, "layout(push_constant) uniform drawBlock {\n");
 utstring_printf(s, "  uint currentFrameInFlight;\n");
  utstring_printf(s, "} draw;\n");
}
void glsl_add_vulkan_global_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {
  utstring_printf(s, "struct globalStruct {\n");
 utstring_printf(s, "  mat4 viewMat ;\n");
 utstring_printf(s, "  mat4 projMat ;\n");
 utstring_printf(s, "  uint directionalLightCount ;\n");
 utstring_printf(s, "  vulkan_directional_light_helper_struct directionalLights [MAX_DIRECTIONAL_LIGHT_COUNT];\n");
 utstring_printf(s, "  uint pointLightCount ;\n");
 utstring_printf(s, "  vulkan_point_light_helper_struct pointLights [MAX_POINT_LIGHT_COUNT];\n");
 utstring_printf(s, "  vulkan_skybox_helper_struct skybox ;\n");
 utstring_printf(s, "  vulkan_font_helper_struct font ;\n");
 utstring_printf(s, "  vulkan_viewport_helper_struct viewport ;\n");
  utstring_printf(s, "};\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform globalBlock {\n", set, binding);
  utstring_printf(s, "  globalStruct global");
  if (count > 1) {utstring_printf(s, "[%u]", count);}
  utstring_printf(s, ";\n};\n");
}
void glsl_add_vulkan_instances_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {
  utstring_printf(s, "struct instancesStruct {\n");
 utstring_printf(s, "  mat4 modelMat ;\n");
 utstring_printf(s, "  uint materialId ;\n");
  utstring_printf(s, "};\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform instancesBlock {\n", set, binding);
  utstring_printf(s, "  instancesStruct instances");
  if (count > 1) {utstring_printf(s, "[%u]", count);}
  utstring_printf(s, ";\n};\n");
}
void glsl_add_vulkan_materials_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {
  utstring_printf(s, "struct materialsStruct {\n");
 utstring_printf(s, "  uint baseColorTextureId ;\n");
 utstring_printf(s, "  vec4 baseColorFactor ;\n");
 utstring_printf(s, "  uint metallicRoughnessTextureId ;\n");
 utstring_printf(s, "  float metallicFactor ;\n");
 utstring_printf(s, "  float roughnessFactor ;\n");
  utstring_printf(s, "};\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform materialsBlock {\n", set, binding);
  utstring_printf(s, "  materialsStruct materials");
  if (count > 1) {utstring_printf(s, "[%u]", count);}
  utstring_printf(s, ";\n};\n");
}
void glsl_add_vulkan_directional_light_helper_struct(UT_string *s) {
  utstring_printf(s, "struct vulkan_directional_light_helper_struct {\n");
 utstring_printf(s, "  vec3 direction;\n");
 utstring_printf(s, "  vec3 color;\n");
  utstring_printf(s, "};\n");
}
void glsl_add_vulkan_font_helper_struct(UT_string *s) {
  utstring_printf(s, "struct vulkan_font_helper_struct {\n");
 utstring_printf(s, "  uint fontTextureId;\n");
  utstring_printf(s, "};\n");
}
void glsl_add_vulkan_point_light_helper_struct(UT_string *s) {
  utstring_printf(s, "struct vulkan_point_light_helper_struct {\n");
 utstring_printf(s, "  vec3 position;\n");
 utstring_printf(s, "  vec3 color;\n");
 utstring_printf(s, "  float radius;\n");
  utstring_printf(s, "};\n");
}
void glsl_add_vulkan_skybox_helper_struct(UT_string *s) {
  utstring_printf(s, "struct vulkan_skybox_helper_struct {\n");
 utstring_printf(s, "  uint skyboxCubemapTextureId;\n");
  utstring_printf(s, "};\n");
}
void glsl_add_vulkan_viewport_helper_struct(UT_string *s) {
  utstring_printf(s, "struct vulkan_viewport_helper_struct {\n");
 utstring_printf(s, "  uint width;\n");
 utstring_printf(s, "  uint height;\n");
  utstring_printf(s, "};\n");
}

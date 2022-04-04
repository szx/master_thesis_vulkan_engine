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
 utstring_printf(s, "  mat4 viewMat;\n");
 utstring_printf(s, "  mat4 projMat;\n");
  utstring_printf(s, "};\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform globalBlock {\n", set, binding);
  utstring_printf(s, "  globalStruct global");
  if (count > 1) {utstring_printf(s, "[%u]", count);}
  utstring_printf(s, ";\n};\n");
}
void glsl_add_vulkan_instances_uniform_buffer(UT_string *s, uint32_t set, uint32_t binding, uint32_t count) {
  utstring_printf(s, "struct instancesStruct {\n");
 utstring_printf(s, "  mat4 modelMat;\n");
 utstring_printf(s, "  uint materialId;\n");
  utstring_printf(s, "};\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform instancesBlock {\n", set, binding);
  utstring_printf(s, "  instancesStruct instances");
  if (count > 1) {utstring_printf(s, "[%u]", count);}
  utstring_printf(s, ";\n};\n");
}

#include "shader_generator.h"

vulkan_shader_generator *vulkan_shader_generator_create(uint32_t *spvCode, size_t spvSize) {
  vulkan_shader_generator *reflect = core_alloc(sizeof(vulkan_shader_generator));

  // HIRO: Complete.

  vulkan_shader_generator_debug_print(reflect, 0);
  return reflect;
}

void vulkan_shader_generator_destroy(vulkan_shader_generator *reflect) { core_free(reflect); }

void vulkan_shader_generator_debug_print(vulkan_shader_generator *reflect, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader generator:", INDENT_FORMAT_ARGS(0));
  // HIRO: Complete.
}

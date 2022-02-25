#include "shader_generator.h"

void glsl_add_header(UT_string *s) { utstring_printf(s, "%s\n", "#version 450"); }

// HIRO input and output variables

void glsl_add_uniform_buffers(UT_string *s, vulkan_unified_uniform_buffer *uniformBuffer) {
  uint32_t set = 0;
  uint32_t binding = 0;
#define str_uniform_buffer(_name, ...)                                                             \
  glsl_add_vulkan_##_name##_uniform_buffer(s, set++, binding, uniformBuffer->_name##Data->count);
  VULKAN_UNIFORM_BUFFERS(str_uniform_buffer, )
#undef str_uniform_buffer
}

void glsl_add_entry_point_begin(UT_string *s) { utstring_printf(s, "%s\n", "void main() {\n"); }

void glsl_add_entry_point_end(UT_string *s) { utstring_printf(s, "%s\n", "}"); }

vulkan_shader_generator *vulkan_shader_generator_create(vulkan_render_state *renderState) {
  vulkan_shader_generator *shaderGenerator = core_alloc(sizeof(vulkan_shader_generator));

  // Generate shaders.
  UT_string *s;

  utstring_new(s);
  glsl_add_header(s);
  glsl_add_uniform_buffers(s, renderState->unifiedUniformBuffer);
  glsl_add_entry_point_begin(s);
  // HIRO vertex shader body
  glsl_add_entry_point_end(s);
  shaderGenerator->vertexShader =
      vulkan_shader_create_with_str(renderState->vkd, vulkan_shader_type_vertex, s);

  utstring_clear(s);
  glsl_add_header(s);
  glsl_add_uniform_buffers(s, renderState->unifiedUniformBuffer);
  glsl_add_entry_point_begin(s);
  // HIRO fragment shader body
  glsl_add_entry_point_end(s);
  shaderGenerator->fragmentShader =
      vulkan_shader_create_with_str(renderState->vkd, vulkan_shader_type_fragment, s);

  // HIRO Generate descriptor set
  // vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_count(renderState->vertexStream);

  /*
  utarray_alloc(shader->descriptorSetLayouts, shader->reflect->maxDescriptorSetNumber);
  utarray_foreach_elem_deref (VkDescriptorSetLayout *, layout, shader->descriptorSetLayouts) {
    *layout = VK_NULL_HANDLE;
  }
  utarray_foreach_elem_deref (vulkan_shader_reflect_binding *, binding,
                              shader->reflect->descriptorBindings) {
    verify(binding->binding == 0); // NOTE: Only one binding per descriptor set.

  uint32_t setNumber = binding->setNumber;
VkDescriptorSetLayout *descriptorSetLayout =
  utarray_eltptr(shader->descriptorSetLayouts, setNumber);

VkDescriptorSetLayoutBinding layoutBinding =
  vulkan_shader_reflect_binding_get_layout_binding(binding);

*descriptorSetLayout =
  vulkan_create_descriptor_set_layout(shader->vkd, &layoutBinding, 1, "shader");
}
*/
  // TODO: Deferred renderer.
  return shaderGenerator;
}

void vulkan_shader_generator_destroy(vulkan_shader_generator *shaderGenerator) {
  core_free(shaderGenerator);
}

void vulkan_shader_generator_debug_print(vulkan_shader_generator *shaderGenerator, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader generator:", INDENT_FORMAT_ARGS(0));
  // HIRO: Complete.
}

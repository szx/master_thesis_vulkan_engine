#include "shader_generator.h"

vulkan_shader_generator *vulkan_shader_generator_create(vulkan_render_state *renderState) {
  vulkan_shader_generator *reflect = core_alloc(sizeof(vulkan_shader_generator));

  // TODO: Deferred renderer.

  // vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_count(renderState->interleavedVertexStream);

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
  return reflect;
}

void vulkan_shader_generator_destroy(vulkan_shader_generator *shaderGenerator) {
  core_free(shaderGenerator);
}

void vulkan_shader_generator_debug_print(vulkan_shader_generator *shaderGenerator, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader generator:", INDENT_FORMAT_ARGS(0));
  // HIRO: Complete.
}

#include "pipeline.h"

vulkan_pipeline *vulkan_pipeline_create(vulkan_device *vkd, data_asset_db *assetDb,
                                        vulkan_batches *batches, vulkan_data_camera *camera,
                                        vulkan_unified_geometry_buffer *unifiedGeometryBuffer,
                                        vulkan_unified_uniform_buffer *unifiedUniformBuffer) {
  vulkan_pipeline *pipeline = core_alloc(sizeof(vulkan_pipeline));

  utarray_alloc(pipeline->shaders, sizeof(vulkan_shader *));
  // HIRO: generate shaders

  // HIRO: create descriptor set layouts from shader reflect
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
  return pipeline;
}

void vulkan_pipeline_destroy(vulkan_pipeline *pipeline) {
  utarray_foreach_elem_deref (vulkan_shader *, shader, pipeline->shaders) {
    vulkan_shader_destroy(shader);
  }
  core_free(pipeline);
}

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline) {
  // HIRO: Complete
}

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline) {
  log_debug("PIPELINE:\n");
  utarray_foreach_elem_deref (vulkan_shader *, shader, pipeline->shaders) {
    vulkan_shader_debug_print(shader, 2);
  }
}

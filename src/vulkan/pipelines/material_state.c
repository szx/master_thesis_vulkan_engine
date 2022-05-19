#include "material_state.h"

vulkan_pipeline_material_state *
vulkan_pipeline_material_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_material_state *materials = core_alloc(sizeof(vulkan_pipeline_material_state));

  materials->renderState = renderState;

  return materials;
}

void vulkan_pipeline_material_state_destroy(vulkan_pipeline_material_state *materials) {
  core_free(materials);
}

void vulkan_pipeline_material_state_reinit_with_new_swap_chain(
    vulkan_pipeline_material_state *material) {
  // No-op.
}

void vulkan_pipeline_material_state_update(vulkan_pipeline_material_state *material) {
  // No-op.
  // TODO: Update material positions?
}

void vulkan_pipeline_material_state_set_material_elements(
    vulkan_pipeline_material_state *materials, uint32_t *materialCount,
    vulkan_material_helper_element *materialElements) {

  *materialCount = HASH_COUNT(materials->renderState->textures->materialElements);

  uthash_foreach_elem_it(vulkan_textures_material_element *, materialElement,
                         materials->renderState->textures->materialElements) {
    size_t materialId = materialElement->materialIdx;
    // PERF: Update material only once (either keep track here or just iterate on
    // textures->materialElements).
    vulkan_material_helper_element *element = &materialElements[materialId];
    element->baseColorTextureId = materialElement->baseColorTextureElement->textureIdx;
    glm_vec4_copy(materialElement->material->baseColorFactor, element->baseColorFactor);
    element->metallicRoughnessTextureId =
        materialElement->metallicRoughnessTextureElement->textureIdx;
    element->metallicFactor = materialElement->material->metallicFactor;
    element->roughnessFactor = materialElement->material->roughnessFactor;
  }
}

void vulkan_pipeline_material_state_debug_print(vulkan_pipeline_material_state *materials,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "materials:", INDENT_FORMAT_ARGS(0));
}

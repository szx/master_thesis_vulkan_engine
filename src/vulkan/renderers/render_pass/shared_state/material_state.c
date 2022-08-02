#include "material_state.h"

render_pass_material_state *render_pass_material_state_create(render_state *renderState) {
  render_pass_material_state *materials = core_alloc(sizeof(render_pass_material_state));

  materials->renderState = renderState;

  return materials;
}

void render_pass_material_state_destroy(render_pass_material_state *materials) {
  core_free(materials);
}

void render_pass_material_state_reinit_with_new_swap_chain(render_pass_material_state *material) {
  // No-op.
}

void render_pass_material_state_update(render_pass_material_state *material) {
  // No-op.
  // TODO: Update material positions?
}

void render_pass_material_state_set_material_elements(render_pass_material_state *materials,
                                                      uint32_t *materialCount,
                                                      material_helper_element *materialElements) {

  *materialCount = HASH_COUNT(materials->renderState->textures->materialElements);

  uthash_foreach_elem_it(textures_material_element *, materialElement,
                         materials->renderState->textures->materialElements) {
    size_t materialId = materialElement->materialIdx;
    // PERF: Update material only once (either keep track here or just iterate on
    // textures->materialElements).
    material_helper_element *element = &materialElements[materialId];
    element->baseColorTextureId = materialElement->baseColorTextureElement->textureIdx;
    glm_vec4_copy(materialElement->material->baseColorFactor, element->baseColorFactor);
    element->metallicRoughnessTextureId =
        materialElement->metallicRoughnessTextureElement->textureIdx;
    element->normalMapTextureId = materialElement->normalMapTextureElement->textureIdx;
    element->metallicFactor = materialElement->material->metallicFactor;
    element->roughnessFactor = materialElement->material->roughnessFactor;
  }
}

void render_pass_material_state_debug_print(render_pass_material_state *materials, int indent) {
  log_debug(INDENT_FORMAT_STRING "materials:", INDENT_FORMAT_ARGS(0));
}

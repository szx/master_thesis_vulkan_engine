#include "light_state.h"

render_pass_light_state *render_pass_light_state_create(render_state *renderState) {
  render_pass_light_state *lights = core_alloc(sizeof(render_pass_light_state));

  lights->renderState = renderState;

  return lights;
}

void render_pass_light_state_destroy(render_pass_light_state *lights) { core_free(lights); }

void render_pass_light_state_reinit_with_new_swap_chain(render_pass_light_state *light) {
  // No-op.
}

void render_pass_light_state_update(render_pass_light_state *light) {
  // No-op.
  // TODO: Oscillate light intensity using time?
}

renderer_cache_direct_light_element *render_pass_light_state_select(render_pass_light_state *lights,
                                                                    direct_light_type type,
                                                                    size_t lightIdx) {
  size_t idx = 0;
loop_start:
  dl_foreach_elem(renderer_cache_direct_light_element *, directLightElement,
                  lights->renderState->rendererCache->directLightElements) {
    if (directLightElement->directLight->type == type) {
      if (lightIdx == idx) {
        return directLightElement;
      }
      idx++;
    }
  }
  if (lightIdx >= idx && idx > 0) {
    lightIdx = lightIdx % idx;
    goto loop_start;
  } else {
    return NULL;
  }
}

void render_pass_light_state_set_direct_light_elements(
    render_pass_light_state *lights, uint32_t *directionalLightCount,
    directional_light_helper_element *directionalLightElements, uint32_t *pointLightCount,
    point_light_helper_element *pointLightElements) {

  *directionalLightCount = 0;
  *pointLightCount = 0;
  dl_foreach_elem(renderer_cache_direct_light_element *, directLightElement,
                  lights->renderState->rendererCache->directLightElements) {
    asset_direct_light *directLight = directLightElement->directLight;

    if (directLight->type == direct_light_type_directional) {
      directional_light_helper_element *directionalLightElement =
          &directionalLightElements[(*directionalLightCount)++];
      glm_vec3_copy(directLight->direction, directionalLightElement->direction);
      directionalLightElement->intensity = directLight->intensity;
      glm_vec3_copy(directLight->color, directionalLightElement->color);
    } else if (directLight->type == direct_light_type_point) {
      point_light_helper_element *pointLightElement = &pointLightElements[(*pointLightCount)++];
      glm_vec3_copy(directLight->position, pointLightElement->position);
      pointLightElement->range = directLight->range;
      pointLightElement->intensity = directLight->intensity;
      glm_vec3_copy(directLight->color, pointLightElement->color);
    } else {
      UNREACHABLE;
    }
  }
}

void render_pass_light_state_debug_print(render_pass_light_state *lights, int indent) {
  log_debug(INDENT_FORMAT_STRING "lights:", INDENT_FORMAT_ARGS(0));
}

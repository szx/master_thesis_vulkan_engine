#include "light_state.h"

vulkan_pipeline_light_state *vulkan_pipeline_light_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_light_state *lights = core_alloc(sizeof(vulkan_pipeline_light_state));

  lights->renderState = renderState;

  return lights;
}

void vulkan_pipeline_light_state_destroy(vulkan_pipeline_light_state *lights) { core_free(lights); }

void vulkan_pipeline_light_state_reinit_with_new_swap_chain(vulkan_pipeline_light_state *light) {
  // No-op.
}

void vulkan_pipeline_light_state_update(vulkan_pipeline_light_state *light) {
  // No-op.
  // TODO: Oscillate light intensity using time?
}

vulkan_renderer_cache_direct_light_element *
vulkan_pipeline_light_state_select(vulkan_pipeline_light_state *lights,
                                   vulkan_direct_light_type type, size_t lightIdx) {
  size_t idx = 0;
loop_start:
  dl_foreach_elem(vulkan_renderer_cache_direct_light_element *, directLightElement,
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

void vulkan_pipeline_light_state_set_direct_light_elements(
    vulkan_pipeline_light_state *lights, uint32_t *directionalLightCount,
    vulkan_directional_light_helper_element *directionalLightElements, uint32_t *pointLightCount,
    vulkan_point_light_helper_element *pointLightElements, uint32_t *spotLightCount,
    vulkan_spot_light_helper_element *spotLightElements) {

  *directionalLightCount = 0;
  *pointLightCount = 0;
  *spotLightCount = 0;
  dl_foreach_elem(vulkan_renderer_cache_direct_light_element *, directLightElement,
                  lights->renderState->rendererCache->directLightElements) {
    vulkan_asset_direct_light *directLight = directLightElement->directLight;

    if (directLight->type == vulkan_direct_light_type_directional) {
      vulkan_directional_light_helper_element *directionalLightElement =
          &directionalLightElements[(*directionalLightCount)++];
      glm_vec3_copy(directLight->direction, directionalLightElement->direction);
      directionalLightElement->intensity = directLight->intensity;
      glm_vec3_copy(directLight->color, directionalLightElement->color);
    } else if (directLight->type == vulkan_direct_light_type_point) {
      vulkan_point_light_helper_element *pointLightElement =
          &pointLightElements[(*pointLightCount)++];
      glm_vec3_copy(directLight->position, pointLightElement->position);
      pointLightElement->range = directLight->range;
      pointLightElement->intensity = directLight->intensity;
      glm_vec3_copy(directLight->color, pointLightElement->color);
    } else if (directLight->type == vulkan_direct_light_type_spot) {
      vulkan_spot_light_helper_element *spotLightElement = &spotLightElements[(*spotLightCount)++];
      glm_vec3_copy(directLight->position, spotLightElement->position);
      spotLightElement->innerConeAngle = directLight->innerConeAngle;
      spotLightElement->outerConeAngle = directLight->outerConeAngle;
      spotLightElement->range = directLight->range;
      spotLightElement->intensity = directLight->intensity;
      glm_vec3_copy(directLight->color, spotLightElement->color);
    } else {
      UNREACHABLE;
    }
  }
}

void vulkan_pipeline_light_state_debug_print(vulkan_pipeline_light_state *lights, int indent) {
  log_debug(INDENT_FORMAT_STRING "lights:", INDENT_FORMAT_ARGS(0));
}

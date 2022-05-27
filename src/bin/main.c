#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double fps, double dt) {
  vulkan_device *vkd = renderer->vkd;
  vulkan_render_state *renderState = renderer->renderState;
  vulkan_pipeline_state *pipelineState = renderer->pipelineState;
  vulkan_pipeline_camera_state *cameraState = renderer->pipelineState->sharedState.camera;
  vulkan_pipeline_light_state *lightState = renderer->pipelineState->sharedState.lights;
  vulkan_pipeline_skybox_state *skyboxState = renderer->pipelineState->sharedState.skybox;
  vulkan_pipeline_font_state *fontState = renderer->pipelineState->sharedState.font;

  if (renderer->vkd->input.keyboard.release.esc) {
    vulkan_renderer_exit_main_loop(renderer);
  }

  /* camera */
  if (renderer->vkd->input.keyboard.release.leftBracket) {
    size_t cameraIdx = cameraState->cameraIdx - 1;
    if (cameraState->cameraIdx == 0) {
      cameraIdx = dl_count(renderState->rendererCache->cameraElements) - 1;
    }
    vulkan_pipeline_camera_state_select(cameraState, cameraIdx);
  }
  if (renderer->vkd->input.keyboard.release.rightBracket) {
    vulkan_pipeline_camera_state_select(cameraState, cameraState->cameraIdx + 1);
  }

  /* lights */
  if (renderer->vkd->input.keyboard.release.p) {
    vulkan_renderer_cache_direct_light_element *directionalLightElement =
        vulkan_pipeline_light_state_select(lightState, vulkan_direct_light_type_directional, 0);
    if (directionalLightElement == NULL) {
      directionalLightElement = vulkan_renderer_cache_direct_light_element_create(
          vulkan_asset_direct_light_create_directional_light(
              lightState->renderState->rendererCache->sceneData, (vec3){0, 0, 0}, 10,
              (vec3){1, 1, 1}));
      vulkan_renderer_cache_add_direct_light_element(lightState->renderState->rendererCache,
                                                     directionalLightElement);
    }
    if (glm_vec3_eqv(directionalLightElement->directLight->direction, (vec3){0, -1, 0})) {
      glm_vec3_copy((vec3){0, 1, 0}, directionalLightElement->directLight->direction);
    } else if (glm_vec3_eqv(directionalLightElement->directLight->direction, (vec3){0, 1, 0})) {
      glm_vec3_copy((vec3){0, 0, -1}, directionalLightElement->directLight->direction);
    } else if (glm_vec3_eqv(directionalLightElement->directLight->direction, (vec3){0, 0, -1})) {
      glm_vec3_copy((vec3){0, 0, 1}, directionalLightElement->directLight->direction);
    } else {
      glm_vec3_copy((vec3){0, -1, 0}, directionalLightElement->directLight->direction);
    }
  }
  if (renderer->vkd->input.keyboard.release.i) {
    vec3 position;
    vulkan_pipeline_camera_state_set_position(cameraState, position);

    vulkan_renderer_cache_direct_light_element *pointLightElement =
        vulkan_renderer_cache_direct_light_element_create(
            vulkan_asset_direct_light_create_point_light(
                lightState->renderState->rendererCache->sceneData, position, 10, 10,
                (vec3){1, 1, 1}));
    vulkan_renderer_cache_add_direct_light_element(lightState->renderState->rendererCache,
                                                   pointLightElement);
  }
  {
    static size_t pointLightIdx = 0;
    vulkan_renderer_cache_direct_light_element *pointLightElement =
        vulkan_pipeline_light_state_select(lightState, vulkan_direct_light_type_point,
                                           pointLightIdx);
    if (pointLightElement != NULL) {
      glm_vec3_copy((vec3){1, 0, 0}, pointLightElement->directLight->color);
      if (renderer->vkd->input.keyboard.release.o) {
        vulkan_pipeline_camera_state_set_position(cameraState,
                                                  pointLightElement->directLight->position);
      }
      float rangeSpeed = 100.0f;
      if (renderer->vkd->input.keyboard.press.num6) {
        pointLightElement->directLight->range -= rangeSpeed * dt;
      }
      if (renderer->vkd->input.keyboard.press.num7) {
        pointLightElement->directLight->range += rangeSpeed * dt;
      }
    } else {
      pointLightIdx = 0;
    }
    if (pointLightIdx > 0 && renderer->vkd->input.keyboard.release.num8) {
      if (pointLightElement != NULL) {
        glm_vec3_copy((vec3){1, 1, 1}, pointLightElement->directLight->color);
      }
      pointLightIdx--;
    }
    if (renderer->vkd->input.keyboard.release.num9) {
      if (pointLightElement != NULL) {
        glm_vec3_copy((vec3){1, 1, 1}, pointLightElement->directLight->color);
      }
      pointLightIdx++;
    }
  }

  /* camera */
  float moveSpeed = cameraState->user.moveSpeed;
  if (renderer->vkd->input.keyboard.press.leftShift) {
    moveSpeed *= 10.0f;
  }
  if (renderer->vkd->input.keyboard.press.w) {
    vulkan_pipeline_camera_state_move(cameraState, moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.s) {
    vulkan_pipeline_camera_state_move(cameraState, -moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.d) {
    vulkan_pipeline_camera_state_move(cameraState, 0.0f, moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.a) {
    vulkan_pipeline_camera_state_move(cameraState, 0.0f, -moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.q) {
    vulkan_pipeline_camera_state_move(cameraState, 0.0f, 0.0f, moveSpeed * dt);
  }
  if (renderer->vkd->input.keyboard.press.e) {
    vulkan_pipeline_camera_state_move(cameraState, 0.0f, 0.0f, -moveSpeed * dt);
  }

  float offsetX = vkd->input.mouse.x - vkd->input.mouse.lastX;
  float offsetY = vkd->input.mouse.y - vkd->input.mouse.lastY;
  if (offsetX != 0.0f || offsetY != 0.0f) {
    float yawDt = offsetX * vkd->input.mouse.sensitivity * cameraState->user.rotationSpeed * dt;
    float pitchDt = offsetY * vkd->input.mouse.sensitivity * cameraState->user.rotationSpeed * dt;
    vulkan_pipeline_camera_state_rotate(cameraState, yawDt, pitchDt, 0.0f);
  }
  vkd->input.mouse.lastX = vkd->input.mouse.x;
  vkd->input.mouse.lastY = vkd->input.mouse.y;

  /* text */
  if (renderer->vkd->input.keyboard.release.t) {
    fontState->debugTextEnabled = !fontState->debugTextEnabled;
  }
  utstring_clear(fontState->text);
  if (fontState->debugTextEnabled) {
    utstring_printf(fontState->text, "FPS: %.2f\ndt: %.2f", fps, dt);
  }

  /* save scane */
  if (renderer->vkd->input.keyboard.release.tab) {
    vulkan_scene_data_serialize(renderState->rendererCache->sceneData, renderer->assetDb);
  }
}

int main(int argc, char *argv[]) {
  platform_create();
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_pipeline_type pipelines[] = {vulkan_pipeline_type_deferred_geometry,
                                      vulkan_pipeline_type_deferred_lighting,
                                      // vulkan_pipeline_type_forward,
                                      vulkan_pipeline_type_skybox, vulkan_pipeline_type_debug_text};
  vulkan_renderer *renderer = vulkan_renderer_create(
      config, assetDb, vks, config->asset.settingsStartScene, pipelines, array_size(pipelines));

  vulkan_renderer_run_main_loop(renderer, update_func);

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  platform_destroy();
  return 0;
}

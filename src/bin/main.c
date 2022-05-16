#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double dt) {
  static bool firstFrame = true;
  vulkan_device *vkd = renderer->vkd;
  vulkan_render_state *renderState = renderer->renderState;
  vulkan_pipeline_state *pipelineState = renderer->pipelineState;
  vulkan_pipeline_camera_state *cameraState = renderer->pipelineState->sharedState.camera;
  vulkan_pipeline_light_state *lightState = renderer->pipelineState->sharedState.lights;
  vulkan_pipeline_skybox_state *skyboxState = renderer->pipelineState->sharedState.skybox;

  if (renderer->vkd->input.keyboard.release.esc) {
    // TODO: Move to vulkan_renderer_quit.
    glfwSetWindowShouldClose(renderer->vkd->window, true);
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
  if (firstFrame) {
    vulkan_directional_light_helper_element *defaultLight =
        vulkan_pipeline_light_state_get_directional_light_element(lightState, 0);
    assert(defaultLight != NULL);
  }
  if (renderer->vkd->input.keyboard.release.l) {
    vulkan_point_light_helper_element *defaultLight =
        vulkan_pipeline_light_state_get_point_light_element(lightState, 0);
    assert(defaultLight != NULL);
    vulkan_pipeline_camera_state_set_position(cameraState, defaultLight->position);
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

  if (firstFrame) {
    firstFrame = false;
  }
}

int main(int argc, char *argv[]) {
  platform_create();
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_pipeline_type pipelines[] = {vulkan_pipeline_type_skybox, vulkan_pipeline_type_forward,
                                      vulkan_pipeline_type_debug_text};
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

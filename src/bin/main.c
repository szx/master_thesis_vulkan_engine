#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double dt) {
  static bool firstFrame = true;

  vulkan_device *vkd = renderer->vkd;
  vulkan_render_state *renderState = renderer->renderState;
  if (renderer->vkd->input.keyboard.release.esc) {
    // TODO: Move to vulkan_renderer_quit.
    glfwSetWindowShouldClose(renderer->vkd->window, true);
  }

  /* lights */
  if (renderer->vkd->input.keyboard.release.leftBracket) {
    size_t cameraIdx = renderState->camera->cameraIdx - 1;
    if (renderState->camera->cameraIdx == 0) {
      cameraIdx = utarray_len(renderState->camera->renderCacheList->cameraRenderCaches) - 1;
    }
    vulkan_camera_select(renderState->camera, cameraIdx);
  }
  if (renderer->vkd->input.keyboard.release.rightBracket) {
    vulkan_camera_select(renderState->camera, renderState->camera->cameraIdx + 1);
  }

  if (firstFrame) {
    vulkan_directional_light_helper_element *defaultLight =
        vulkan_lights_get_directional_light_element(renderState->lights, 0);
    assert(defaultLight != NULL);
  }
  if (renderer->vkd->input.keyboard.release.l) {
    vulkan_point_light_helper_element *defaultLight =
        vulkan_lights_get_point_light_element(renderState->lights, 0);
    assert(defaultLight != NULL);
    vulkan_camera_set_position(renderState->camera, defaultLight->position);
  }

  /* camera */
  float moveSpeed = renderState->camera->user.moveSpeed;
  if (renderer->vkd->input.keyboard.press.leftShift) {
    moveSpeed *= 10.0f;
  }
  if (renderer->vkd->input.keyboard.press.w) {
    vulkan_camera_move(renderState->camera, moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.s) {
    vulkan_camera_move(renderState->camera, -moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.d) {
    vulkan_camera_move(renderState->camera, 0.0f, moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.a) {
    vulkan_camera_move(renderState->camera, 0.0f, -moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.q) {
    vulkan_camera_move(renderState->camera, 0.0f, 0.0f, moveSpeed * dt);
  }
  if (renderer->vkd->input.keyboard.press.e) {
    vulkan_camera_move(renderState->camera, 0.0f, 0.0f, -moveSpeed * dt);
  }

  float offsetX = vkd->input.mouse.x - vkd->input.mouse.lastX;
  float offsetY = vkd->input.mouse.y - vkd->input.mouse.lastY;
  if (offsetX != 0.0f || offsetY != 0.0f) {
    float yawDt =
        offsetX * vkd->input.mouse.sensitivity * renderState->camera->user.rotationSpeed * dt;
    float pitchDt =
        offsetY * vkd->input.mouse.sensitivity * renderState->camera->user.rotationSpeed * dt;
    vulkan_camera_rotate(renderState->camera, yawDt, pitchDt, 0.0f);
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
  vulkan_pipeline_type pipelines[] = {vulkan_pipeline_type_skybox, vulkan_pipeline_type_forward};
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

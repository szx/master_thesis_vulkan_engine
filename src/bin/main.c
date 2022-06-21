#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double fps, double dt) {
  vulkan_device *vkd = renderer->vkd;
  vulkan_render_state *renderState = renderer->renderState;
  vulkan_render_pass_state *renderPassState = renderer->renderGraph->renderPassState;
  vulkan_render_pass_camera_state *cameraState = renderPassState->sharedState.camera;
  vulkan_render_pass_light_state *lightState = renderPassState->sharedState.lights;
  vulkan_render_pass_skybox_state *skyboxState = renderPassState->sharedState.skybox;
  vulkan_render_pass_font_state *fontState = renderPassState->sharedState.font;

  if (renderer->vkd->input.keyboard.release.esc) {
    vulkan_renderer_exit_main_loop(renderer);
  }

  /* camera */
  if (renderer->vkd->input.keyboard.release.leftBracket) {
    size_t cameraIdx = cameraState->cameraIdx - 1;
    if (cameraState->cameraIdx == 0) {
      cameraIdx = dl_count(renderState->rendererCache->cameraElements) - 1;
    }
    vulkan_render_pass_camera_state_select(cameraState, cameraIdx);
  }
  if (renderer->vkd->input.keyboard.release.rightBracket) {
    vulkan_render_pass_camera_state_select(cameraState, cameraState->cameraIdx + 1);
  }

  /* lights */
  if (renderer->vkd->input.keyboard.release.p) {
    vulkan_renderer_cache_direct_light_element *directionalLightElement =
        vulkan_render_pass_light_state_select(lightState, vulkan_direct_light_type_directional, 0);
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
    vulkan_render_pass_camera_state_set_position(cameraState, position);

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
        vulkan_render_pass_light_state_select(lightState, vulkan_direct_light_type_point,
                                              pointLightIdx);
    if (pointLightElement != NULL) {
      glm_vec3_copy((vec3){1, 0, 0}, pointLightElement->directLight->color);
      if (renderer->vkd->input.keyboard.release.o) {
        vulkan_render_pass_camera_state_set_position(cameraState,
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
    vulkan_render_pass_camera_state_move(cameraState, moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.s) {
    vulkan_render_pass_camera_state_move(cameraState, -moveSpeed * dt, 0.0f, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.d) {
    vulkan_render_pass_camera_state_move(cameraState, 0.0f, moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.a) {
    vulkan_render_pass_camera_state_move(cameraState, 0.0f, -moveSpeed * dt, 0.0f);
  }
  if (renderer->vkd->input.keyboard.press.q) {
    vulkan_render_pass_camera_state_move(cameraState, 0.0f, 0.0f, moveSpeed * dt);
  }
  if (renderer->vkd->input.keyboard.press.e) {
    vulkan_render_pass_camera_state_move(cameraState, 0.0f, 0.0f, -moveSpeed * dt);
  }

  float offsetX = vkd->input.mouse.x - vkd->input.mouse.lastX;
  float offsetY = vkd->input.mouse.y - vkd->input.mouse.lastY;
  if (offsetX != 0.0f || offsetY != 0.0f) {
    float yawDt = offsetX * vkd->input.mouse.sensitivity * cameraState->user.rotationSpeed * dt;
    float pitchDt = offsetY * vkd->input.mouse.sensitivity * cameraState->user.rotationSpeed * dt;
    vulkan_render_pass_camera_state_rotate(cameraState, yawDt, pitchDt, 0.0f);
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

void render_pass_record_primitive_geometry_draws(vulkan_render_pass *renderPass,
                                                 vulkan_render_pass_frame_state *frameState,
                                                 VkCommandBuffer commandBuffer) {
  vulkan_batches_record_draw_command(renderPass->renderPassState->sharedState.rendererCacheBatches,
                                     commandBuffer, &frameState->rendererCacheBatchesData);
}

void render_pass_record_fullscreen_triangle_draw(vulkan_render_pass *renderPass,
                                                 vulkan_render_pass_frame_state *frameState,
                                                 VkCommandBuffer commandBuffer) {
  // Draw full-screen triangle.
  // See also:
  // https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
  vulkan_renderer_cache_primitive_element *basicFullscreenTrianglePrimitiveElement =
      renderPass->renderPassState->renderState->rendererCache
          ->basicFullscreenTrianglePrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicFullscreenTrianglePrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

void render_pass_record_skybox_draw(vulkan_render_pass *renderPass,
                                    vulkan_render_pass_frame_state *frameState,
                                    VkCommandBuffer commandBuffer) {
  // Draw cube.
  vulkan_renderer_cache_primitive_element *basicBoxPrimitiveElement =
      renderPass->renderPassState->renderState->rendererCache->basicBoxPrimitiveElement;
  vulkan_batch batch;
  vulkan_batch_init(&batch, basicBoxPrimitiveElement, 0);
  vulkan_batch_record_basic_draw_command(&batch, commandBuffer);
  vulkan_batch_deinit(&batch);
}

int main(int argc, char *argv[]) {
  platform_create(argc, argv);
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_renderer *renderer =
      vulkan_renderer_create(config, assetDb, vks, config->asset.settingsStartScene);

  // TODO: HDR rendering (requires support for loading floating-point skyboxes)
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "depthBuffer",
                                         vulkan_image_type_offscreen_depth_buffer);
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "gBuffer0",
                                         vulkan_image_type_offscreen_f16);
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "gBuffer1",
                                         vulkan_image_type_offscreen_f16);
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "gBuffer2",
                                         vulkan_image_type_offscreen_f16);
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "ssaoRaw",
                                         vulkan_image_type_offscreen_r8);
  vulkan_render_graph_add_image_resource(renderer->renderGraph, "ssaoBlurred",
                                         vulkan_image_type_offscreen_r8);

  /*
  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){.vertexShader = "forward_vertex.glsl",
                                .fragmentShader = "forward_fragment.glsl",
                                .useOnscreenColorAttachment = true,
                                .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                                .offscreenColorAttachmentCount = 0,
                                .useDepthAttachment = true,
                                .depthAttachmentWriteEnable = true,
                                .depthAttachmentTestEnable = true,
                                .depthAttachmentTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                                .depthClearValue = (VkClearDepthStencilValue){0.0f, 0},
                                .colorBlendingType = vulkan_color_blending_type_none,
                                .recordFunc = render_pass_record_primitive_geometry_draws});
  */

  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){
          .vertexShader = "deferred_geometry_vertex.glsl",
          .fragmentShader = "deferred_geometry_fragment.glsl",
          .useOnscreenColorAttachment = false,
          .onscreenClearValue = {{0.0f, 0.0f, 0.0f, 1.0f}},
          .offscreenColorAttachmentCount = 3,
          .offscreenColorAttachments =
              {
                  {.name = "gBuffer0", .clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}}},
                  {.name = "gBuffer1", .clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}}},
                  {.name = "gBuffer2", .clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}}},
              },
          .offscreenDepthAttachment =
              {
                  .name = "depthBuffer",
                  .depthWriteEnable = true,
                  .depthTestEnable = true,
                  .depthTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                  .clearValue = {0.0f, 0},
              },
          .colorBlendingType = vulkan_color_blending_type_none,
          .recordFunc = render_pass_record_primitive_geometry_draws});

  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){
          .vertexShader = "ssao_vertex.glsl",
          .fragmentShader = "ssao_fragment.glsl",
          .forceEarlyFragmentTests = true,
          .offscreenFragmentShaderInputCount = 2,
          .offscreenFragmentShaderInputs =
              {
                  {.name = "gBuffer0"},
                  {.name = "gBuffer2"},
              },
          .offscreenColorAttachmentCount = 1,
          .offscreenColorAttachments =
              {
                  {.name = "ssaoRaw", .clearValue = {{1.0f, 0.0f, 0.0f, 1.0f}}},
              },
          .offscreenDepthAttachment =
              {
                  // Use depth buffer to reject fragments with depth == 0 (no geometry rendered)
                  .name = "depthBuffer",
                  .depthWriteEnable = false,
                  .depthTestEnable = true,
                  .depthTestOp = VK_COMPARE_OP_LESS,
              },
          .colorBlendingType = vulkan_color_blending_type_none,
          .recordFunc = render_pass_record_fullscreen_triangle_draw});

  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){
          .vertexShader = "deferred_lighting_vertex.glsl",
          .fragmentShader = "deferred_lighting_fragment.glsl",
          .forceEarlyFragmentTests = true,
          .useOnscreenColorAttachment = true,
          .offscreenFragmentShaderInputCount = 4,
          .offscreenFragmentShaderInputs =
              {
                  {.name = "gBuffer0"},
                  {.name = "gBuffer1"},
                  {.name = "gBuffer2"},
                  {.name = "ssaoRaw"},
              },
          .offscreenDepthAttachment =
              {
                  // Use depth buffer to reject fragments with depth == 0 (no geometry rendered)
                  .name = "depthBuffer",
                  .depthWriteEnable = false,
                  .depthTestEnable = true,
                  .depthTestOp = VK_COMPARE_OP_LESS,
              },
          .recordFunc = render_pass_record_fullscreen_triangle_draw});

  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){.vertexShader = "skybox_vertex.glsl",
                                .fragmentShader = "skybox_fragment.glsl",
                                .useOnscreenColorAttachment = true,
                                .onscreenClearValue = {{0.5f, 0.5f, 0.5f, 1.0f}},
                                .offscreenDepthAttachment =
                                    {
                                        // Use depth buffer to reject fragments with depth != 0
                                        .name = "depthBuffer",
                                        .depthWriteEnable = false,
                                        .depthTestEnable = true,
                                        .depthTestOp = VK_COMPARE_OP_EQUAL,
                                    },
                                .recordFunc = render_pass_record_skybox_draw});

  vulkan_render_graph_add_render_pass(
      renderer->renderGraph,
      (vulkan_render_pass_desc){.vertexShader = "debug_text_vertex.glsl",
                                .fragmentShader = "debug_text_fragment.glsl",
                                .useOnscreenColorAttachment = true,
                                .colorBlendingType = vulkan_color_blending_type_alpha,
                                .recordFunc = render_pass_record_fullscreen_triangle_draw});

  vulkan_renderer_run_main_loop(renderer, update_func);

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  platform_destroy();
  return 0;
}

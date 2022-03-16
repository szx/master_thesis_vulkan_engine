#include "renderer.h"

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_swap_chain *vks,
                                                    UT_string *sceneName) {
  vulkan_scene_renderer *renderer = core_alloc(sizeof(vulkan_scene_renderer));

  renderer->assetDb = assetDb;
  renderer->data = vulkan_data_scene_create_with_asset_db(renderer->assetDb, sceneName);

  data_config *config = data_config_create();
  renderer->renderCacheList = vulkan_render_cache_list_create(config->graphicsMaxInstanceCount);
  data_config_destroy(config);

  renderer->sceneGraph = vulkan_scene_graph_create(renderer->data, renderer->renderCacheList);
  vulkan_data_camera_init(&renderer->camera, renderer->data);
  vulkan_data_camera_copy(&renderer->camera, utarray_back(renderer->data->cameras));
  utarray_back(renderer->data->cameras);

  renderer->vkd = vks->vkd;
  renderer->vks = vks;
  renderer->renderState =
      vulkan_render_state_create(renderer->vkd, renderer->renderCacheList, &renderer->camera);
  renderer->pipeline = vulkan_pipeline_create(renderer->vks, renderer->renderState);

  return renderer;
}

void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer) {
  vulkan_pipeline_destroy(renderer->pipeline);
  vulkan_render_state_destroy(renderer->renderState);

  vulkan_scene_graph_destroy(renderer->sceneGraph);
  vulkan_render_cache_list_destroy(renderer->renderCacheList);
  vulkan_data_scene_destroy(renderer->data);

  core_free(renderer);
}

void vulkan_scene_renderer_update(vulkan_scene_renderer *renderer) {
  vulkan_scene_render_state_update(renderer->renderState);
}

void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer) {
  vulkan_render_state_send_to_device(renderer->renderState);
}

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer) {
  log_debug("SCENE RENDERER:\n");
  vulkan_data_scene_debug_print(renderer->data);
  vulkan_render_cache_list_debug_print(renderer->renderCacheList);
  vulkan_scene_graph_debug_print(renderer->sceneGraph);
  vulkan_render_state_debug_print(renderer->renderState);
  vulkan_pipeline_debug_print(renderer->pipeline);
}

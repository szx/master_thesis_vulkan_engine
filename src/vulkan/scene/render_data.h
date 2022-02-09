/* Vulkan scene graph.
 * Used to build scene tree.
 */
#pragma once

#include "tree.h"

#include "../device.h"
#include "../functions.h"
#include "../geometry_buffer.h"

// HIRO Draw call batching? Split into batch_list and render_context?

/// Creates and destroys Vulkan objects used to draw scene described by
/// scene data objects (see scene_data).
typedef struct vulkan_scene_render_data {
  data_asset_db *assetDb; /// data_asset_db pointer
  vulkan_data_scene *data;
  vulkan_scene_graph *sceneGraph;
  vulkan_scene_tree *sceneTree;

  vulkan_device *vkd; /// vulkan_device pointer
  vulkan_geometry_buffer *geometryBuffer;
  vulkan_uniform_buffer *uniformBuffer;
} vulkan_scene_render_data;

// HIRO sceneName to sceneData/sceneGraph/sceneTree/sceneObjectList/sceneBatches
vulkan_scene_render_data *vulkan_scene_render_data_create(data_asset_db *assetDb,
                                                          vulkan_device *vkd, UT_string *sceneName);
void vulkan_scene_render_data_destroy(vulkan_scene_render_data *renderData);

void vulkan_scene_render_data_build_geometry_buffer(
    vulkan_scene_render_data *renderData); // TODO move to separate header
/// Update scene data on CPU. Can make camera and uniform buffer dirty.
void vulkan_scene_render_data_update_data(vulkan_scene_render_data *renderData);
void vulkan_scene_render_data_send_to_device(vulkan_scene_render_data *renderData);

void vulkan_scene_render_data_debug_print(vulkan_scene_render_data *renderData);

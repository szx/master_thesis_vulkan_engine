/* Vulkan scene */
#pragma once

#include "../core/platform.h"

#include "device.h"
#include "functions.h"
#include "geometry_buffer.h"
#include "scene_data.h"

// HIRO SceneGraph, SceneTree, ShapeList, Batches?

/// Creates and destroys Vulkan objects used to draw scene described by
/// scene data objects (see scene_data).
typedef struct vulkan_scene {
  data_asset_db *assetDb; /// data_asset_db pointer
  vulkan_device *vkd;     /// vulkan_device pointer
  vulkan_scene_data *data;
  vulkan_geometry_buffer *geometryBuffer;
  vulkan_uniform_buffer *uniformBuffer;
} vulkan_scene;

vulkan_scene *vulkan_scene_create(data_asset_db *assetDb, vulkan_device *vkd, UT_string *sceneName);
void vulkan_scene_destroy(vulkan_scene *scene);
void vulkan_scene_debug_print(vulkan_scene *scene);

void vulkan_scene_build_geometry_buffer(vulkan_scene *scene); // TODO move to separate header
/// Update scene data on CPU. Can make camera and uniform buffer dirty.
void vulkan_scene_update_data(vulkan_scene *scene);
void vulkan_scene_send_to_device(vulkan_scene *scene);

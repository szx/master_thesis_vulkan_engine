/* Vulkan scene and glTF parsing */
#pragma once

#include "../core/platform.h"

#include "device.h"
#include "functions.h"
#include "scene_data.h"

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_render_context).
typedef struct vulkan_scene {
  vulkan_device *vkd; /// vulkan_device pointer
  vulkan_scene_data *data;
  vulkan_geometry_buffer *geometryBuffer;
  vulkan_uniform_buffer *uniformBuffer;
  // TODO VK_EXT_vertex_input_dynamic_state is not supported by Intel.
  // TODO VK_EXT_extended_dynamic_state allows VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT
  // TODO Save and load scene from disk.
} vulkan_scene;

vulkan_scene *vulkan_scene_create(vulkan_device *vkd, UT_string *sceneName);
void vulkan_scene_destroy(vulkan_scene *scene);
void vulkan_scene_debug_print(vulkan_scene *scene);

void vulkan_scene_build_geometry_buffer(vulkan_scene *scene); // TODO move to separate header
/// Update scene data on CPU. Can make camera and uniform buffer dirty.
void vulkan_scene_update_data(vulkan_scene *scene);
void vulkan_scene_send_to_device(vulkan_scene *scene);

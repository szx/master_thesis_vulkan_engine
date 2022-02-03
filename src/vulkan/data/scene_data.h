/* Basic scene data.
 * glTF parsing and loading scene from asset database. */
#pragma once

#include "../../data/data.h"
#include "camera.h"
#include "mesh.h"
#include "node.h"
#include "primitive.h"

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_scene).
typedef struct vulkan_scene_data {
  UT_string *name;
  UT_array *primitives; /// vulkan_primitive_data array, all primitives used by scene.
  UT_array *nodes;      /// vulkan_node_data array, all nodes in scene.
  UT_array *cameras;    /// vulkan_camera_data array, all cameras in scene
  bool dirty;           /// True if scene data updated on CPU.
  data_key hash;        /// Hash, used to prevent duplicates in asset database.
} vulkan_scene_data;

vulkan_scene_data *vulkan_scene_data_create(UT_string *name);
void vulkan_scene_data_destroy(vulkan_scene_data *sceneData);
void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData);

/// Adds new primitive. Returns integer index.
vulkan_primitive_data_index vulkan_scene_data_add_primitive(vulkan_scene_data *sceneData,
                                                            vulkan_primitive_data primitive);

/* asset pipeline */
vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath);
vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);

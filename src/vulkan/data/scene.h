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
  vulkan_material_data *materials;   /// Doubly-linked list, all materials used by scene.
  vulkan_primitive_data *primitives; /// Double-linked list, all primitives used by scene.
  UT_array *nodes;                   /// vulkan_node_data array, all nodes in scene.
  UT_array *cameras;                 /// vulkan_camera_data array, all cameras in scene
  bool dirty;                        /// True if scene data updated on CPU.
  data_key hash;                     /// Hash, used to prevent duplicates in asset database.
} vulkan_scene_data;

vulkan_scene_data *vulkan_scene_data_create(UT_string *name);
void vulkan_scene_data_destroy(vulkan_scene_data *sceneData);

data_key vulkan_scene_data_calculate_key(vulkan_scene_data *scene);
void vulkan_scene_data_serialize(vulkan_scene_data *scene, data_asset_db *assetDb);
void vulkan_scene_data_deserialize(vulkan_scene_data *scene, data_asset_db *assetDb, data_key key);

vulkan_material_data *vulkan_scene_data_get_material_by_key(vulkan_scene_data *sceneData,
                                                            data_asset_db *assetDb, data_key key);
vulkan_primitive_data *vulkan_scene_data_get_primitive_by_key(vulkan_scene_data *sceneData,
                                                              data_asset_db *assetDb, data_key key);

/* asset pipeline */
vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath);
vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);

void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData);

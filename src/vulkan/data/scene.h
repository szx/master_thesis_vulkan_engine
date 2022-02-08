/* Basic scene data.
 * glTF parsing and loading scene from asset database. */
#pragma once

#include "../../data/data.h"
#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "primitive.h"

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_scene).
typedef struct vulkan_data_scene {
  UT_string *name;
  UT_string *path;                   /// Empty when loaded from asset db.
  vulkan_data_image *images;         /// Doubly-linked list, all images used by scene.
  vulkan_data_sampler *samplers;     /// Doubly-linked list, all samplers used by scene.
  vulkan_data_texture *textures;     /// Doubly-linked list, all textures used by scene.
  vulkan_data_material *materials;   /// Doubly-linked list, all materials used by scene.
  vulkan_data_primitive *primitives; /// Doubly-linked list, all primitives used by scene.
  vulkan_data_object *objects;       /// Doubly-linked list, all objects in scene.
  UT_array *cameras;                 /// vulkan_data_camera array, all cameras in scene
  UT_array *rootObjects; /// vulkan_data_object array, object that are direct descendants of scene
  bool dirty;            /// True if scene data updated on CPU.
  data_key hash;         /// Hash, used to prevent duplicates in asset database.
} vulkan_data_scene;

vulkan_data_scene *vulkan_data_scene_create(UT_string *name);
void vulkan_data_scene_destroy(vulkan_data_scene *sceneData);

data_key vulkan_data_scene_calculate_key(vulkan_data_scene *scene);
void vulkan_data_scene_serialize(vulkan_data_scene *scene, data_asset_db *assetDb);
void vulkan_data_scene_deserialize(vulkan_data_scene *scene, data_asset_db *assetDb, data_key key);

vulkan_data_image *vulkan_data_scene_get_image_by_key(vulkan_data_scene *sceneData,
                                                      data_asset_db *assetDb, data_key key);
vulkan_data_sampler *vulkan_data_scene_get_sampler_by_key(vulkan_data_scene *sceneData,
                                                          data_asset_db *assetDb, data_key key);
vulkan_data_texture *vulkan_data_scene_get_texture_by_key(vulkan_data_scene *sceneData,
                                                          data_asset_db *assetDb, data_key key);
vulkan_data_material *vulkan_data_scene_get_material_by_key(vulkan_data_scene *sceneData,
                                                            data_asset_db *assetDb, data_key key);
vulkan_data_primitive *vulkan_data_scene_get_primitive_by_key(vulkan_data_scene *sceneData,
                                                              data_asset_db *assetDb, data_key key);
vulkan_data_object *vulkan_data_scene_get_object_by_key(vulkan_data_scene *sceneData,
                                                        data_asset_db *assetDb, data_key key);

/* asset pipeline */
vulkan_data_scene *vulkan_data_scene_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath);
vulkan_data_scene *vulkan_data_scene_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);

void vulkan_data_scene_debug_print(vulkan_data_scene *sceneData);

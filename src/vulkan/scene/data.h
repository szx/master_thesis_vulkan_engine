/* Basic scene data.
 * glTF parsing and loading scene from asset database. */
#pragma once

#include "../../data/data.h"
#include "../assets/assets.h"
#include "../common.h"

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_scene).
typedef struct vulkan_scene_data {
  UT_string *name;
  UT_string *path; ///< Empty when loaded from asset db.

  vulkan_asset_image *images;                      ///< List of all images used by scene.
  vulkan_asset_sampler *samplers;                  ///< List of all samplers used by scene.
  vulkan_asset_texture *textures;                  ///< List of all textures used by scene.
  vulkan_asset_skybox *skyboxes;                   ///< List of all skyboxes used by scene.
  vulkan_asset_material *materials;                ///< List of all materials used by scene.
  vulkan_asset_vertex_attribute *vertexAttributes; ///< List of all vertex attributes used by scene.
  vulkan_asset_primitive *primitives;              ///< List of all primitives used by scene.
  vulkan_asset_object *objects;                    ///< List of all objects in scene.

  /// vulkan_asset_object* array, object that are direct descendants of scene
  UT_array *rootObjects;
  /// Skybox/
  vulkan_asset_skybox *skybox;
  // HIRO HIRO light data (ambientIntensity)?

  /// Default camera.
  vulkan_asset_camera defaultCamera;

  data_key key; ///< Hash, used to prevent duplicates in asset database.
} vulkan_scene_data;

vulkan_scene_data *vulkan_scene_data_create(UT_string *name);
void vulkan_scene_data_destroy(vulkan_scene_data *sceneData);

data_key vulkan_scene_data_calculate_key(vulkan_scene_data *scene);
void vulkan_scene_data_serialize(vulkan_scene_data *scene, data_asset_db *assetDb);
void vulkan_scene_data_deserialize(vulkan_scene_data *scene, data_asset_db *assetDb, data_key key);

/* get_*_by_key() and add_*() */
#define GET_VULKAN_ASSET_BY_KEY_DECL(_type)                                                        \
  vulkan_asset_##_type *vulkan_scene_data_get_##_type##_by_key(                                    \
      vulkan_scene_data *sceneData, data_asset_db *assetDb, data_key key);

#define ADD_VULKAN_ASSET_DECL(_type)                                                               \
  vulkan_asset_##_type *vulkan_scene_data_add_##_type(vulkan_scene_data *sceneData,                \
                                                      vulkan_asset_##_type *entity);

#define VULKAN_SCENE_ASSETS_DECLS(_type)                                                           \
  GET_VULKAN_ASSET_BY_KEY_DECL(_type) ADD_VULKAN_ASSET_DECL(_type)

VULKAN_SCENE_ASSETS_DECLS(image)
VULKAN_SCENE_ASSETS_DECLS(sampler)
VULKAN_SCENE_ASSETS_DECLS(texture)
VULKAN_SCENE_ASSETS_DECLS(skybox)
VULKAN_SCENE_ASSETS_DECLS(material)
VULKAN_SCENE_ASSETS_DECLS(vertex_attribute)
VULKAN_SCENE_ASSETS_DECLS(primitive)
VULKAN_SCENE_ASSETS_DECLS(object)

#undef GET_VULKAN_ASSET_BY_KEY_DECL
#undef ADD_VULKAN_ASSET_DECL
#undef VULKAN_SCENE_ASSETS_DECLS

/* asset pipeline */
vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath,
                                                           UT_string *configPath,
                                                           data_asset_db *assetDb);
vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);

void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData, int indent);

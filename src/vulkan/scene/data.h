/* Basic scene data.
 * glTF parsing and loading scene from asset database. */
#pragma once

#include "../../data/data.h"
#include "../assets/assets.h"
#include "../common.h"

/// Describes a scene.
/// Does not create any Vulkan objects (see scene).
typedef struct scene_data {
  data_config *assetConfig; ///< Pointer.

  UT_string *name;
  UT_string *path; ///< Empty when loaded from asset db.

  asset_image *images;                      ///< List of all images used by scene.
  asset_sampler *samplers;                  ///< List of all samplers used by scene.
  asset_texture *textures;                  ///< List of all textures used by scene.
  asset_direct_light *directLights;         ///< List of all direct lights used by scene.
  asset_skybox *skyboxes;                   ///< List of all skyboxes used by scene.
  asset_font *fonts;                        ///< List of all fonts used by scene.
  asset_material *materials;                ///< List of all materials used by scene.
  asset_vertex_attribute *vertexAttributes; ///< List of all vertex attributes used by scene.
  asset_primitive *primitives;              ///< List of all primitives used by scene.
  asset_object *objects;                    ///< List of all objects in scene.

  /// asset_object* array, object that are direct descendants of scene
  UT_array *rootObjects;
  /// Skybox.
  asset_skybox *skybox;
  /// Font.
  asset_font *font;
  /// Default camera.
  asset_camera defaultCamera;

  data_key key; ///< Hash, used to prevent duplicates in asset database.
} scene_data;

scene_data *scene_data_create(UT_string *name, data_config *assetConfig);
void scene_data_destroy(scene_data *sceneData);

data_key scene_data_calculate_key(scene_data *scene);
void scene_data_serialize(scene_data *scene, asset_db *assetDb);
void scene_data_deserialize(scene_data *scene, asset_db *assetDb, data_key key);

#define GET_VULKAN_ASSET_BY_KEY_DECL(_type)                                                        \
  asset_##_type *scene_data_get_##_type##_by_key(scene_data *sceneData, asset_db *assetDb,         \
                                                 data_key key);

#define GET_VULKAN_DEFAULT_ASSET(_type)                                                            \
  asset_##_type *scene_data_get_default_##_type(scene_data *sceneData);

#define ADD_VULKAN_ASSET_DECL(_type)                                                               \
  asset_##_type *scene_data_add_##_type(scene_data *sceneData, asset_##_type *entity);

#define VULKAN_SCENE_ASSETS_DECLS(_type)                                                           \
  GET_VULKAN_ASSET_BY_KEY_DECL(_type)                                                              \
  GET_VULKAN_DEFAULT_ASSET(_type) ADD_VULKAN_ASSET_DECL(_type)

VULKAN_SCENE_ASSETS_DECLS(image)
VULKAN_SCENE_ASSETS_DECLS(sampler)
VULKAN_SCENE_ASSETS_DECLS(texture)
VULKAN_SCENE_ASSETS_DECLS(direct_light)
VULKAN_SCENE_ASSETS_DECLS(skybox)
VULKAN_SCENE_ASSETS_DECLS(font)
VULKAN_SCENE_ASSETS_DECLS(material)
VULKAN_SCENE_ASSETS_DECLS(vertex_attribute)
VULKAN_SCENE_ASSETS_DECLS(primitive)
VULKAN_SCENE_ASSETS_DECLS(object)

#undef GET_VULKAN_ASSET_BY_KEY_DECL
#undef ADD_VULKAN_ASSET_DECL
#undef VULKAN_SCENE_ASSETS_DECLS

/* asset pipeline */
scene_data *scene_data_create_with_gltf_file(UT_string *sceneName, UT_string *gltfPath,
                                             UT_string *sceneConfigPath, data_config *assetConfig,
                                             asset_db *assetDb);
scene_data *scene_data_create_with_asset_db(data_config *assetConfig, asset_db *assetDb,
                                            UT_string *sceneName);

void scene_data_debug_print(scene_data *sceneData, int indent);

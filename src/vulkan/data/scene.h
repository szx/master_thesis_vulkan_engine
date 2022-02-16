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
  UT_string *path;                                /// Empty when loaded from asset db.
  vulkan_data_image *images;                      /// List of all images used by scene.
  vulkan_data_sampler *samplers;                  /// List of all samplers used by scene.
  vulkan_data_texture *textures;                  /// List of all textures used by scene.
  vulkan_data_material *materials;                /// List of all materials used by scene.
  vulkan_data_vertex_attribute *vertexAttributes; /// List of all vertex attributes used by scene.
  vulkan_data_primitive *primitives;              /// List of all primitives used by scene.
  vulkan_data_object *objects;                    /// List of all objects in scene.
  UT_array *cameras;                              /// vulkan_data_camera array, all cameras in scene
  UT_array *rootObjects; /// vulkan_data_object* array, object that are direct descendants of scene

  data_key key; /// Hash, used to prevent duplicates in asset database.
} vulkan_data_scene;

vulkan_data_scene *vulkan_data_scene_create(UT_string *name);
void vulkan_data_scene_destroy(vulkan_data_scene *sceneData);

data_key vulkan_data_scene_calculate_key(vulkan_data_scene *scene);
void vulkan_data_scene_serialize(vulkan_data_scene *scene, data_asset_db *assetDb);
void vulkan_data_scene_deserialize(vulkan_data_scene *scene, data_asset_db *assetDb, data_key key);

/* get_*_by_key() and add_*() */
#define DECL_GET_VULKAN_ENTITY_BY_KEY(_type)                                                       \
  vulkan_data_##_type *vulkan_data_scene_get_##_type##_by_key(                                     \
      vulkan_data_scene *sceneData, data_asset_db *assetDb, data_key key);

#define DECL_ADD_VULKAN_ENTITY(_type)                                                              \
  vulkan_data_##_type *vulkan_data_scene_add_##_type(vulkan_data_scene *sceneData,                 \
                                                     vulkan_data_##_type *entity);

#define DECL_VULKAN_ENTITY_FUNCS(_type)                                                            \
  DECL_GET_VULKAN_ENTITY_BY_KEY(_type) DECL_ADD_VULKAN_ENTITY(_type)

DECL_VULKAN_ENTITY_FUNCS(image)
DECL_VULKAN_ENTITY_FUNCS(sampler)
DECL_VULKAN_ENTITY_FUNCS(texture)
DECL_VULKAN_ENTITY_FUNCS(material)
DECL_VULKAN_ENTITY_FUNCS(vertex_attribute)
DECL_VULKAN_ENTITY_FUNCS(primitive)
DECL_VULKAN_ENTITY_FUNCS(object)

#undef DECL_GET_VULKAN_ENTITY_BY_KEY
#undef DECL_ADD_VULKAN_ENTITY
#undef DECL_VULKAN_ENTITY_FUNCS

/* asset pipeline */
vulkan_data_scene *vulkan_data_scene_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath);
vulkan_data_scene *vulkan_data_scene_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);

void vulkan_data_scene_debug_print(vulkan_data_scene *sceneData);

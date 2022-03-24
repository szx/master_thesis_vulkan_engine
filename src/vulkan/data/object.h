/* Object state.
 * Represents one object in scene.
 * Contains one mesh and transform.
 * Used by scene data.
 */
#pragma once

#include "../objects/vertex_stream.h"
#include "camera.h"
#include "common.h"
#include "mesh.h"

typedef struct vulkan_data_object {
  vulkan_data_mesh *mesh;     /// Can be NULL if object is only transforms children.
  vulkan_data_camera *camera; /// Can be NULL if object has no attached camera.
  mat4 transform;
  UT_array *children; /// vulkan_data_object* array

  DECL_VULKAN_ENTITY(object)
} vulkan_data_object;

void vulkan_data_object_init(vulkan_data_object *object, vulkan_data_scene *sceneData);
void vulkan_data_object_deinit(vulkan_data_object *object);

data_key vulkan_data_object_calculate_key(vulkan_data_object *object);
void vulkan_data_object_serialize(vulkan_data_object *object, data_asset_db *assetDb);
void vulkan_data_object_deserialize(vulkan_data_object *object, data_asset_db *assetDb,
                                    data_key key);

void vulkan_data_object_debug_print(vulkan_data_object *object, int indent);

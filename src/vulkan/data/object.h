/* Object state.
 * Represents one object in scene.
 * Contains one mesh and transform.
 * Used by scene data.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"
#include "mesh.h"

typedef struct vulkan_data_object {
  vulkan_data_scene *sceneData; // vulkan_data_scene pointer
  vulkan_data_mesh mesh;
  mat4 transform;
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_data_object;

void vulkan_data_object_init(vulkan_data_object *object, vulkan_data_scene *sceneData);
void vulkan_data_object_deinit(vulkan_data_object *object);

data_key vulkan_data_object_calculate_key(vulkan_data_object *object);
void vulkan_data_object_serialize(vulkan_data_object *object, data_asset_db *assetDb);
void vulkan_data_object_deserialize(vulkan_data_object *object, data_asset_db *assetDb,
                                    data_key key);

void vulkan_data_object_debug_print(vulkan_data_object *object);

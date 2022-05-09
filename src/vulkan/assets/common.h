/* Common scene data.
 */
#pragma once

#include "../../core/core.h"
#include "../../data/data.h"
#include "../common.h"

typedef struct vulkan_scene_data vulkan_scene_data;

#define VULKAN_ASSET_FIELD_DECLS(_name)                                                            \
  data_key key;                                                                                    \
  vulkan_scene_data *sceneData;                                                                    \
  struct vulkan_asset_##_name *prev, *next;

#define VULKAN_ASSET_FIELD_DEFS(_name, _var)                                                       \
  (_var)->sceneData = sceneData;                                                                   \
  (_var)->key = vulkan_asset_##_name##_calculate_key(_var);                                        \
  (_var)->prev = NULL;                                                                             \
  (_var)->next = NULL;

#define VULKAN_ASSET_FUNC_DECLS(_name)                                                             \
  void vulkan_asset_##_name##_init(vulkan_asset_##_name *entity, vulkan_scene_data *sceneData);    \
  void vulkan_asset_##_name##_deinit(vulkan_asset_##_name *entity);                                \
                                                                                                   \
  data_key vulkan_asset_##_name##_calculate_key(vulkan_asset_##_name *entity);                     \
  void vulkan_asset_##_name##_serialize(vulkan_asset_##_name *entity, data_asset_db *assetDb);     \
  void vulkan_asset_##_name##_deserialize(vulkan_asset_##_name *entity, data_asset_db *assetDb,    \
                                          data_key key);                                           \
                                                                                                   \
  void vulkan_asset_##_name##_debug_print(vulkan_asset_##_name *entity, int indent);

/* Common scene data.
 */
#pragma once

#include "../../core/core.h"
#include "../../data/data.h"
#include "../common.h"

typedef struct scene_data scene_data;

#define VULKAN_ASSET_FIELD_DECLS(_name)                                                            \
  data_key key;                                                                                    \
  scene_data *sceneData;                                                                           \
  struct asset_##_name *prev, *next;

#define VULKAN_ASSET_FIELD_DEFS(_name, _var)                                                       \
  (_var)->sceneData = sceneData;                                                                   \
  (_var)->key = asset_##_name##_calculate_key(_var);                                               \
  (_var)->prev = NULL;                                                                             \
  (_var)->next = NULL;

#define VULKAN_ASSET_FUNC_DECLS(_name)                                                             \
  void asset_##_name##_init(asset_##_name *entity, scene_data *sceneData);                         \
  void asset_##_name##_deinit(asset_##_name *entity);                                              \
                                                                                                   \
  data_key asset_##_name##_calculate_key(asset_##_name *entity);                                   \
  void asset_##_name##_serialize(asset_##_name *entity, asset_db *assetDb);                        \
  void asset_##_name##_deserialize(asset_##_name *entity, asset_db *assetDb, data_key key);        \
                                                                                                   \
  void asset_##_name##_debug_print(asset_##_name *entity, int indent);

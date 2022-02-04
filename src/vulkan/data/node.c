#include "node.h"
#include "scene.h"

void vulkan_node_data_init(vulkan_node_data *node, vulkan_scene_data *sceneData) {
  node->sceneData = sceneData;
  glm_mat4_identity(node->transform);
  data_key_init(&node->hash, 0);
}

void vulkan_node_data_deinit(vulkan_node_data *node) { vulkan_mesh_data_deinit(&node->mesh); }

data_key vulkan_node_data_calculate_key(vulkan_node_data *node) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &node->transform, sizeof(node->transform))
  HASH_UPDATE(hashState, &node->mesh.hash, sizeof(node->mesh.hash))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_node_data_serialize(vulkan_node_data *node, data_asset_db *assetDb) {
  node->hash = vulkan_node_data_calculate_key(node);
  vulkan_mesh_data *mesh = &node->mesh;
  vulkan_mesh_data_serialize(mesh, assetDb);

  data_mat4 transformMat;
  glm_mat4_copy(node->transform, transformMat.value);
  data_asset_db_insert_node_transform_mat4(assetDb, node->hash, transformMat);

  data_asset_db_insert_node_mesh_key(assetDb, node->hash, node->mesh.hash);
}

void vulkan_node_data_deserialize(vulkan_node_data *node, data_asset_db *assetDb, data_key key) {
  node->hash = key;

  glm_mat4_copy(data_asset_db_select_node_transform_mat4(assetDb, node->hash).value,
                node->transform);

  data_key meshHash = data_asset_db_select_node_mesh_key(assetDb, node->hash);
  vulkan_mesh_data_init(&node->mesh, node->sceneData);
  vulkan_mesh_data_deserialize(&node->mesh, assetDb, meshHash);
}

void vulkan_node_data_debug_print(vulkan_node_data *node) {
  log_debug("node:\n");
  glm_mat4_print(node->transform, stderr);
  log_debug("  hash=%zu", node->hash);
  vulkan_mesh_data *mesh = &node->mesh;
  vulkan_mesh_data_debug_print(mesh);
}

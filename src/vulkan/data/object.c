#include "object.h"
#include "scene.h"

void vulkan_data_node_init(vulkan_data_node *node, vulkan_data_scene *sceneData) {
  node->sceneData = sceneData;
  glm_mat4_identity(node->transform);
  data_key_init(&node->hash, 0);
}

void vulkan_data_node_deinit(vulkan_data_node *node) { vulkan_data_mesh_deinit(&node->mesh); }

data_key vulkan_data_node_calculate_key(vulkan_data_node *node) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &node->transform, sizeof(node->transform))
  HASH_UPDATE(hashState, &node->mesh.hash, sizeof(node->mesh.hash))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_node_serialize(vulkan_data_node *node, data_asset_db *assetDb) {
  node->hash = vulkan_data_node_calculate_key(node);
  vulkan_data_mesh *mesh = &node->mesh;
  vulkan_data_mesh_serialize(mesh, assetDb);

  data_mat4 transformMat;
  glm_mat4_copy(node->transform, transformMat.value);
  data_asset_db_insert_node_transform_mat4(assetDb, node->hash, transformMat);

  data_asset_db_insert_node_mesh_key(assetDb, node->hash, node->mesh.hash);
}

void vulkan_data_node_deserialize(vulkan_data_node *node, data_asset_db *assetDb, data_key key) {
  node->hash = key;

  glm_mat4_copy(data_asset_db_select_node_transform_mat4(assetDb, node->hash).value,
                node->transform);

  data_key meshHash = data_asset_db_select_node_mesh_key(assetDb, node->hash);
  vulkan_data_mesh_init(&node->mesh, node->sceneData);
  vulkan_data_mesh_deserialize(&node->mesh, assetDb, meshHash);
}

void vulkan_data_node_debug_print(vulkan_data_node *node) {
  log_debug("node:\n");
  glm_mat4_print(node->transform, stderr);
  log_debug("  hash=%zu", node->hash);
  vulkan_data_mesh *mesh = &node->mesh;
  vulkan_data_mesh_debug_print(mesh);
}

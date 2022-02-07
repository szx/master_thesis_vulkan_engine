#include "node.h"

vulkan_scene_node *vulkan_scene_node_create(vulkan_data_scene *data) {
  vulkan_scene_node *sceneNode = core_alloc(sizeof(vulkan_scene_node));
  sceneNode->data = data;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  vulkan_data_scene_destroy(sceneNode->data);
  core_free(sceneNode);
}

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  log_debug("SCENE GRAPH:\n");
  // TODO: Graphviz output.
  vulkan_data_scene_debug_print(sceneNode->data);
}

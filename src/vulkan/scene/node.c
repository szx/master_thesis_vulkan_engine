#include "node.h"

vulkan_scene_node *vulkan_scene_node_create(vulkan_scene_node_type type, void *entity) {
  vulkan_scene_node *sceneNode = core_alloc(sizeof(vulkan_scene_node));

  sceneNode->type = type;
  if (sceneNode->type == vulkan_scene_node_type_object) {
    sceneNode->object = entity;
  } else if (sceneNode->type == vulkan_scene_node_type_mesh) {
    sceneNode->mesh = entity;
  } else if (sceneNode->type == vulkan_scene_node_type_primitive) {
    sceneNode->primitive = entity;
  } else {
    assert(0);
  }

  utarray_alloc(sceneNode->successors, sizeof(vulkan_scene_node *));

  sceneNode->next = NULL;
  sceneNode->next = NULL;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  // NOTE: Successors are freed by scene graph/tree.
  utarray_free(sceneNode->successors);
  core_free(sceneNode);
}

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  log_debug("node: %s %p", vulkan_scene_node_type_debug_str(sceneNode->type), sceneNode);
  // TODO: Graphviz output.
}

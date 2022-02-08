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
  sceneNode->next = NULL;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) { core_free(sceneNode); }

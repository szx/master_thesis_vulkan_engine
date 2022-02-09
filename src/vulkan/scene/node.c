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
  } else if (sceneNode->type != vulkan_scene_node_type_root) {
    assert(0);
  }

  utarray_alloc(sceneNode->successors, sizeof(vulkan_scene_node *));

  sceneNode->prev = NULL;
  sceneNode->next = NULL;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  // NOTE: Successors are freed by scene graph/tree.
  utarray_free(sceneNode->successors);
  core_free(sceneNode);
}

void vulkan_scene_node_add_successor(vulkan_scene_node *sceneNode,
                                     vulkan_scene_node *successorNode) {
  utarray_push_back(sceneNode->successors, &successorNode);
}

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  if (utarray_len(sceneNode->successors) == 0) {
    log_raw(stdout, "%s_%p; ", vulkan_scene_node_type_debug_str(sceneNode->type), sceneNode);
  } else {
    vulkan_scene_node **successorIt = NULL;
    while ((successorIt = utarray_next(sceneNode->successors, successorIt))) {
      vulkan_scene_node *successor = *successorIt;
      log_raw(stdout, "%s_%p -> %s_%p; ", vulkan_scene_node_type_debug_str(sceneNode->type),
              sceneNode, vulkan_scene_node_type_debug_str(successor->type), successor);
      vulkan_scene_node_debug_print(successor);
    }
  }
}

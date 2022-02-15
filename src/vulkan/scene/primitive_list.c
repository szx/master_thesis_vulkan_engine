#include "primitive_list.h"

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_primitive_list *sceneTree = core_alloc(sizeof(vulkan_scene_primitive_list));
  sceneTree->graph = sceneGraph;
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(vulkan_scene_node *));
  return sceneTree;
}

void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  dl_foreach_elem (vulkan_scene_node *, node, sceneTree->nodes) { vulkan_scene_node_destroy(node); }

  core_free(sceneTree);
}

void vulkan_scene_primitive_list_set_dirty(vulkan_scene_primitive_list *sceneTree,
                                           vulkan_scene_node *sceneNode) {
  if (!sceneNode->dirty) {
    sceneNode->dirty = true;
    utarray_push_back(sceneTree->dirtyNodes, &sceneNode);
  }
}

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  vulkan_scene_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
}

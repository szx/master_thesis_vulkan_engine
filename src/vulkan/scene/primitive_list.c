#include "primitive_list.h"

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree) {
  vulkan_scene_primitive_list *primitiveList = core_alloc(sizeof(vulkan_scene_primitive_list));

  primitiveList->sceneTree = sceneTree;
  utarray_alloc(primitiveList->primitives, sizeof(vulkan_scene_node *));

  return primitiveList;
}

void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList) {
  utarray_free(primitiveList->primitives);

  core_free(primitiveList);
}

void vulkan_scene_primitive_list_add_primitive(vulkan_scene_primitive_list *primitiveList,
                                               vulkan_scene_node *primitiveSceneTreeNode) {
  utarray_push_back(primitiveList->primitives, &primitiveSceneTreeNode);
}

void vulkan_scene_primitive_list_remove_primitive(vulkan_scene_primitive_list *primitiveList,
                                                  vulkan_scene_node *primitiveSceneTreeNode) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_scene_node *, primitive, primitiveList->primitives) {
    if (primitive == primitiveSceneTreeNode) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(primitiveList->primitives));
  utarray_erase(primitiveList->primitives, idx, 1);
}

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList) {
  log_raw(stdout, "digraph primitive_list {");
  utarray_foreach_elem_deref (vulkan_scene_node *, primitive, primitiveList->primitives) {
    vulkan_scene_node_debug_print(primitive);
  }
  log_raw(stdout, "}\n");
}

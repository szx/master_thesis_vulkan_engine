#include "primitive_list.h"

vulkan_scene_batch *vulkan_scene_batch_create(vulkan_scene_batch_flags flags) {
  vulkan_scene_batch *batch = core_alloc(sizeof(vulkan_scene_batch));
  batch->flags = flags;
  utarray_alloc(batch->primitiveNodes, sizeof(vulkan_scene_node *));
  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_scene_batch_destroy(vulkan_scene_batch *batch) {
  utarray_free(batch->primitiveNodes);
  core_free(batch);
}

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree) {
  vulkan_scene_primitive_list *primitiveList = core_alloc(sizeof(vulkan_scene_primitive_list));

  primitiveList->sceneTree = sceneTree;
  utarray_alloc(primitiveList->primitiveNodes, sizeof(vulkan_scene_node *));
  primitiveList->batches = NULL;
  return primitiveList;
}

void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList) {
  dl_foreach_elem (vulkan_scene_batch *, batch, primitiveList->batches) {
    vulkan_scene_batch_destroy(batch);
  }
  utarray_free(primitiveList->primitiveNodes);
  core_free(primitiveList);
}

void vulkan_scene_primitive_list_add_primitive(vulkan_scene_primitive_list *primitiveList,
                                               vulkan_scene_node *primitiveSceneTreeNode) {
  utarray_push_back(primitiveList->primitiveNodes, &primitiveSceneTreeNode);
}

void vulkan_scene_primitive_list_remove_primitive(vulkan_scene_primitive_list *primitiveList,
                                                  vulkan_scene_node *primitiveSceneTreeNode) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode, primitiveList->primitiveNodes) {
    if (primitiveNode == primitiveSceneTreeNode) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(primitiveList->primitiveNodes));
  utarray_erase(primitiveList->primitiveNodes, idx, 1);
  // HIRO update batches
}

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList) {
  log_raw(stdout, "digraph primitive_list {");
  utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode, primitiveList->primitiveNodes) {
    vulkan_scene_node_debug_print(primitiveNode);
  }
  log_raw(stdout, "}\n");
}

#include "primitive_list.h"

vulkan_scene_batch *vulkan_scene_batch_create(vulkan_scene_batch_flags flags) {
  vulkan_scene_batch *batch = core_alloc(sizeof(vulkan_scene_batch));
  batch->flags = flags;
  batch->caches = NULL;
  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_scene_batch_destroy(vulkan_scene_batch *batch) {
  // NOTE: Caches are destroyed by scene tree.
  core_free(batch);
}

bool vulkan_scene_batch_matches_cache(vulkan_scene_batch *batch, vulkan_scene_cache *cache) {
  assert(cache->primitive != NULL);

  if (batch->caches == NULL) {
    return true;
  }

  vulkan_scene_cache *batchedNode = batch->caches;
  bool matched = true;
  if ((batch->flags & vulkan_scene_batch_vertex_attributes_flag) != 0) {
    if (!vulkan_data_primitive_vulkan_attributes_match(cache->primitive, batchedNode->primitive)) {
      matched = false;
    }
  } else if ((batch->flags & vulkan_scene_batch_material_flag) != 0) {
    if (!vulkan_data_primitive_material_match(cache->primitive, batchedNode->primitive)) {
      matched = false;
    }
  }

  return matched;
}

void vulkan_scene_batch_add_cache(vulkan_scene_batch *batch, vulkan_scene_cache *cache) {
  assert(vulkan_scene_batch_matches_cache(batch, cache));
  DL_APPEND(batch->caches, cache);
}

void vulkan_scene_batch_debug_print(vulkan_scene_batch *batch) {
  log_raw(stdout, "digraph batch_%s {", vulkan_scene_batch_flags_debug_str(batch->flags));
  dl_foreach_elem (vulkan_scene_cache *, cache, batch->caches) {
    vulkan_scene_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");
}

vulkan_scene_primitive_list *vulkan_scene_primitive_list_create(vulkan_scene_tree *sceneTree) {
  vulkan_scene_primitive_list *primitiveList = core_alloc(sizeof(vulkan_scene_primitive_list));

  primitiveList->sceneTree = sceneTree;
  utarray_alloc(primitiveList->caches, sizeof(vulkan_scene_node *));
  primitiveList->batches = NULL;
  return primitiveList;
}

void vulkan_scene_primitive_list_destroy(vulkan_scene_primitive_list *primitiveList) {
  dl_foreach_elem (vulkan_scene_batch *, batch, primitiveList->batches) {
    vulkan_scene_batch_destroy(batch);
  }
  utarray_free(primitiveList->caches);
  core_free(primitiveList);
}

void vulkan_scene_primitive_list_add_cache(vulkan_scene_primitive_list *primitiveList,
                                           vulkan_scene_cache *cache) {
  utarray_push_back(primitiveList->caches, &cache);

  // add primitive node to first matching batch
  bool foundBatch = false;
  dl_foreach_elem (vulkan_scene_batch *, batch, primitiveList->batches) {
    if (vulkan_scene_batch_matches_cache(batch, cache)) {
      vulkan_scene_batch_add_cache(batch, cache);
      foundBatch = true;
      break;
    }
  }
  if (!foundBatch) {
    vulkan_scene_batch *newBatch = vulkan_scene_batch_create(vulkan_scene_batch_vertex_all_flag);
    DL_APPEND(primitiveList->batches, newBatch);
    vulkan_scene_batch_add_cache(newBatch, cache);
  }
}

void vulkan_scene_primitive_list_remove_cache(vulkan_scene_primitive_list *primitiveList,
                                              vulkan_scene_cache *cache) {
  size_t idx = 0;
  utarray_foreach_elem_deref (vulkan_scene_cache *, existingCache, primitiveList->caches) {
    if (existingCache == cache) {
      break;
    }
    idx++;
  }
  assert(idx < utarray_len(primitiveList->caches));
  utarray_erase(primitiveList->caches, idx, 1);
  // HIRO removing cache from batches
}

void vulkan_scene_primitive_list_debug_print(vulkan_scene_primitive_list *primitiveList) {
  log_raw(stdout, "digraph primitive_list {");
  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, primitiveList->caches) {
    vulkan_scene_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");

  dl_foreach_elem (vulkan_scene_batch *, batch, primitiveList->batches) {
    vulkan_scene_batch_debug_print(batch);
  }
}

#include "render_cache.h"
#include "../data/mesh.h"
#include "../data/primitive.h"
#include "../scene/node.h"

vulkan_render_cache *vulkan_render_cache_create(vulkan_scene_tree_node *sceneTreeNode) {
  vulkan_render_cache *renderCache = core_alloc(sizeof(vulkan_render_cache));

  renderCache->node = sceneTreeNode;
  renderCache->prev = NULL;
  renderCache->next = NULL;

  vulkan_render_cache_set_with_node(renderCache);
  renderCache->firstIndexOffset = 0;
  renderCache->firstVertexOffset = 0;
  renderCache->renderCacheListIdx = 0;

  return renderCache;
}

void vulkan_render_cache_destroy(vulkan_render_cache *renderCache) { core_free(renderCache); }

void vulkan_render_cache_set_with_node(vulkan_render_cache *renderCache) {
  renderCache->distanceFromRoot = 0;

  renderCache->visible = true;

  if (renderCache->node->object != NULL) {
    glm_mat4_copy(renderCache->node->object->transform, renderCache->transform);
    renderCache->mesh = renderCache->node->object->mesh;
    if (renderCache->node->object->camera) {
      vulkan_data_camera_copy(&renderCache->camera, renderCache->node->object->camera);
    } else {
      vulkan_data_camera_init(&renderCache->camera, NULL);
    }
  } else {
    glm_mat4_identity(renderCache->transform);
    renderCache->mesh = NULL;
    vulkan_data_camera_init(&renderCache->camera, NULL);
  }

  renderCache->primitive = renderCache->node->primitive;
}

void vulkan_render_cache_accumulate(vulkan_render_cache *renderCache,
                                    vulkan_render_cache *parentCache) {
  assert(parentCache->node->primitive == NULL);
  vulkan_render_cache_set_with_node(renderCache);
  renderCache->distanceFromRoot = parentCache->distanceFromRoot + 1;
  renderCache->visible = parentCache->visible;
  glm_mat4_mul(parentCache->transform, renderCache->transform, renderCache->transform);
  if (parentCache->mesh != NULL) {
    renderCache->mesh = parentCache->mesh;
  }
  if (renderCache->node->object != NULL && renderCache->node->object->camera != NULL) {
    vulkan_data_camera_copy(&renderCache->camera, renderCache->node->object->camera);
  } else {
    vulkan_data_camera_copy(&renderCache->camera, &parentCache->camera);
  }
}

void vulkan_render_cache_set_vertex_stream_offsets(vulkan_render_cache *renderCache,
                                                   size_t firstIndexOffset,
                                                   size_t firstVertexOffset) {
  renderCache->firstIndexOffset = firstIndexOffset;
  renderCache->firstVertexOffset = firstVertexOffset;
}

void debug_log_cache(vulkan_render_cache *renderCache) {
  log_raw(stdout, "\"cache\\n%p\\n", renderCache->node);
  log_raw(stdout, "distanceFromRoot: %d\\n", renderCache->distanceFromRoot);
  log_raw(stdout, "visible: %d\\n", renderCache->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(renderCache->transform));
  log_raw(stdout, "mesh: %p\\n", renderCache->mesh);
  log_raw(stdout, "cameraFovY %f\\n", renderCache->camera.fovY);
  log_raw(stdout, "primitive: %p\\n", renderCache->primitive);
  log_raw(stdout, "firstIndexOffset: %zu\\n", renderCache->firstIndexOffset);
  log_raw(stdout, "firstVertexOffset: %zu\\n", renderCache->firstVertexOffset);
  log_raw(stdout, "renderCacheListIdx: %zu", renderCache->renderCacheListIdx);
  log_raw(stdout, "\"");
}

void vulkan_render_cache_debug_print(vulkan_render_cache *renderCache) {
  debug_log_cache(renderCache);
  log_raw(stdout, "; ");
}

#include "render_cache.h"

#include "../assets/mesh.h"
#include "../assets/primitive.h"
#include "../scene/node.h"
#include "textures.h"

vulkan_render_cache *vulkan_render_cache_create(vulkan_scene_tree_node *sceneTreeNode) {
  vulkan_render_cache *renderCache = core_alloc(sizeof(vulkan_render_cache));

  renderCache->node = sceneTreeNode;
  renderCache->prev = NULL;
  renderCache->next = NULL;

  vulkan_render_cache_reset(renderCache);
  renderCache->firstIndexOffset = 0;
  renderCache->firstVertexOffset = 0;

  renderCache->instanceId = 0;

  renderCache->materialElement = 0;

  return renderCache;
}

void vulkan_render_cache_destroy(vulkan_render_cache *renderCache) { core_free(renderCache); }

void vulkan_render_cache_reset(vulkan_render_cache *renderCache) {
  renderCache->distanceFromRoot = 0;

  renderCache->visible = true;

  if (renderCache->node != NULL && renderCache->node->object != NULL) {
    glm_mat4_copy(renderCache->node->object->transform, renderCache->transform);
    renderCache->mesh = renderCache->node->object->mesh;
    if (renderCache->node->object->camera) {
      vulkan_asset_camera_copy(&renderCache->camera, renderCache->node->object->camera);
    } else {
      vulkan_asset_camera_init(&renderCache->camera, NULL);
    }
  } else {
    glm_mat4_identity(renderCache->transform);
    renderCache->mesh = NULL;
    vulkan_asset_camera_init(&renderCache->camera, NULL);
  }

  if (renderCache->node != NULL) {
    renderCache->primitive = renderCache->node->primitive;
  } else {
    renderCache->primitive = NULL;
  }

  renderCache->aabb = vulkan_aabb_default();
}

void vulkan_render_cache_accumulate(vulkan_render_cache *renderCache,
                                    vulkan_render_cache *parentCache) {
  assert(parentCache->node->primitive == NULL);
  vulkan_render_cache_reset(renderCache);
  renderCache->distanceFromRoot = parentCache->distanceFromRoot + 1;
  renderCache->visible = parentCache->visible;
  glm_mat4_mul(parentCache->transform, renderCache->transform, renderCache->transform);
  if (parentCache->mesh != NULL) {
    renderCache->mesh = parentCache->mesh;
  }
  if (renderCache->node->object != NULL && renderCache->node->object->camera != NULL) {
    vulkan_asset_camera_copy(&renderCache->camera, renderCache->node->object->camera);
  } else {
    vulkan_asset_camera_copy(&renderCache->camera, &parentCache->camera);
  }
  if (renderCache->primitive != NULL) {
    renderCache->aabb = vulkan_asset_primitive_calculate_aabb(renderCache->primitive);
    glm_mat4_mulv(renderCache->transform, renderCache->aabb.min, renderCache->aabb.min);
    glm_mat4_mulv(renderCache->transform, renderCache->aabb.max, renderCache->aabb.max);
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
  log_raw(stdout, "instanceId: %zu", renderCache->instanceId);
  if (renderCache->materialElement != NULL) {
    log_raw(stdout, "materialId: %zu", renderCache->materialElement->materialIdx);
  }
  log_raw(stdout, "\"");
}

void vulkan_render_cache_debug_print(vulkan_render_cache *renderCache) {
  debug_log_cache(renderCache);
  log_raw(stdout, "; ");
}

#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"
#include "greatest.h"

#include <stdlib.h>

#define GLTF_NAME "cameras"

// Loading sponza.gltf.
TEST gltf_loading() {
  // TODO: Loading extra files (images).
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  UT_string *gltfPath = get_asset_file_path(GLTF_NAME, GLTF_NAME ".gltf");
  vulkan_data_scene *gltfSceneData = vulkan_data_scene_create_with_gltf_file(sceneName, gltfPath);
  vulkan_data_scene *assetDbSceneData = vulkan_data_scene_create_with_asset_db(assetDb, sceneName);

  ASSERT_STR_EQ(utstring_body(gltfSceneData->name), utstring_body(assetDbSceneData->name));
  ASSERT_EQ(gltfSceneData->key.value, assetDbSceneData->key.value);

#define ASSERT_VERTEX_ATTRIBUTE(_name)                                                             \
  ASSERT_EQ(utarray_len(gltfPrimitive->_name->data), utarray_len(assetDbPrimitive->_name->data));  \
  if (utarray_len(gltfPrimitive->_name->data) > 0)                                                 \
    ASSERT_MEM_EQ(utarray_front(gltfPrimitive->_name->data),                                       \
                  utarray_front(assetDbPrimitive->_name->data),                                    \
                  utarray_size(gltfPrimitive->_name->data));

  dl_foreach_elem(vulkan_data_object *, assetDbNode, assetDbSceneData->objects, {
    bool foundCorrespondingNode = false;
    vulkan_data_object *gltfNode;
    dl_foreach_elem(vulkan_data_object *, nodeIter, gltfSceneData->objects, {
      if (nodeIter->key.value == assetDbNode->key.value) {
        foundCorrespondingNode = true;
        gltfNode = nodeIter;
        break;
      }
    })
    ASSERT_EQ(foundCorrespondingNode, true);
    ASSERT_MEM_EQ(gltfNode->transform, assetDbNode->transform, sizeof(mat4));
    ASSERT_EQ(utarray_len(gltfNode->children), utarray_len(assetDbNode->children));
    vulkan_data_mesh *gltfMesh = gltfNode->mesh;
    vulkan_data_mesh *assetDbMesh = assetDbNode->mesh;
    ASSERT((gltfMesh == NULL && assetDbMesh == NULL) ||
           (gltfMesh->key.value == assetDbMesh->key.value));
    vulkan_data_camera *gltfCamera = gltfNode->camera;
    vulkan_data_camera *assetDbCamera = assetDbNode->camera;
    ASSERT((gltfCamera == NULL && assetDbCamera == NULL) ||
           (gltfCamera->key.value == assetDbCamera->key.value));

    bool foundCorrespondingPrimitive = false;
    dl_foreach_elem(vulkan_data_primitive *, gltfPrimitive, gltfSceneData->primitives, {
      vulkan_data_primitive *assetDbPrimitive;
      dl_foreach_elem(vulkan_data_primitive *, primitiveIter, assetDbSceneData->primitives, {
        if (gltfPrimitive->key.value == primitiveIter->key.value) {
          foundCorrespondingPrimitive = true;
          assetDbPrimitive = primitiveIter;
          break;
        }
      })
      ASSERT_EQ(foundCorrespondingPrimitive, true);
      ASSERT_EQ(gltfPrimitive->material->key.value, assetDbPrimitive->material->key.value);
      ASSERT_EQ(gltfPrimitive->topology, assetDbPrimitive->topology);
      ASSERT_EQ(gltfPrimitive->vertexCount, assetDbPrimitive->vertexCount);

      ASSERT_VERTEX_ATTRIBUTE(positions)
      ASSERT_VERTEX_ATTRIBUTE(normals)
      ASSERT_VERTEX_ATTRIBUTE(colors)
      ASSERT_VERTEX_ATTRIBUTE(texCoords)
    })
  })
  utstring_free(gltfPath);
  utstring_free(sceneName);
  vulkan_data_scene_destroy(assetDbSceneData);
  vulkan_data_scene_destroy(gltfSceneData);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  PASS();
}

SUITE(gltf_suite) { RUN_TEST(gltf_loading); }

TEST shaderc_compiling() {
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  vulkan_renderer *renderer = vulkan_renderer_create(config, assetDb, vks, sceneName);
  utstring_free(sceneName);

  vulkan_renderer_debug_print(renderer);

  vulkan_shader *vertexShader = renderer->pipeline->shaderGenerator->vertexShader;
  vulkan_shader *fragmentShader = renderer->pipeline->shaderGenerator->fragmentShader;

  // verify shader
  ASSERT(vertexShader->type == vulkan_shader_type_vertex);
  ASSERT(fragmentShader->type == vulkan_shader_type_fragment);
  ASSERT(vertexShader->spvSize > 0);
  ASSERT(fragmentShader->spvSize > 0);
  ASSERT(vertexShader->module != VK_NULL_HANDLE);
  ASSERT(fragmentShader->module != VK_NULL_HANDLE);

  // verify reflection
  ASSERT(utarray_len(vertexShader->reflect->inputVariables) > 0);
  ASSERT(utarray_len(fragmentShader->reflect->inputVariables) > 0);
  ASSERT(utarray_len(vertexShader->reflect->outputVariables) > 0);
  ASSERT(utarray_len(fragmentShader->reflect->outputVariables) > 0);
  // HIRO check in vertex output variables match fragment input variables

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);

  PASS();
}

SUITE(shaderc_suite) { RUN_TEST(shaderc_compiling); }

void assert_graph(vulkan_scene_graph *sceneGraph) {

  vulkan_scene_graph_debug_print(sceneGraph); /* scene tree */
  vulkan_scene_tree *sceneTree = sceneGraph->sceneTree;
  assert(utarray_len(sceneTree->dirtyNodes) == 0);
  size_t primitiveNodeNum = 0;
  dl_foreach_elem_new(vulkan_scene_tree_node *, sceneNode,
                      sceneTree->nodes) { /*log_debug("tree node: %p", sceneNode);*/
    if (sceneNode->object != NULL) {
      assert(utarray_len(sceneNode->childNodes) > 0 || sceneNode->object->camera != NULL);
    }
    if (sceneNode->primitive != NULL) {
      assert(utarray_len(sceneNode->childNodes) == 0);
      primitiveNodeNum++;
    }
    assert(!sceneNode->dirty);
  }
  /* cache list */
  vulkan_render_cache_list *renderCacheList = sceneTree->renderCacheList;
  assert(utarray_len(renderCacheList->caches) == primitiveNodeNum);
}

// Building scene graph.
TEST scene_graph_building() {
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  vulkan_data_scene *assetDbSceneData = vulkan_data_scene_create_with_asset_db(assetDb, sceneName);
  // vulkan_data_scene_debug_print(assetDbSceneData);
  vulkan_render_cache_list *renderCacheList =
      vulkan_render_cache_list_create(config->graphicsMaxInstanceCount);
  vulkan_scene_graph *sceneGraph = vulkan_scene_graph_create(assetDbSceneData, renderCacheList);

  log_info("Verify cache accumulation.");
  vulkan_scene_graph_node *firstObjectNode =
      *(vulkan_scene_graph_node **)utarray_front(sceneGraph->root->childNodes);
  assert(firstObjectNode != NULL);
  assert(firstObjectNode->object != NULL);
  firstObjectNode->object->transform[0][0] = 2;
  vulkan_scene_graph_set_dirty(sceneGraph, firstObjectNode);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);
  assert_graph(sceneGraph);

  vulkan_scene_tree_node *sceneTreeNode =
      *(vulkan_scene_tree_node **)utarray_front(firstObjectNode->observers);
  while (sceneTreeNode != NULL) {
    ASSERT_EQ(sceneTreeNode->renderCache->transform[0][0], 2);
    if (utarray_len(sceneTreeNode->childNodes) > 0) {
      sceneTreeNode = *(vulkan_scene_tree_node **)utarray_front(sceneTreeNode->childNodes);
    } else {
      sceneTreeNode = NULL;
    }
  }

  // Verify adding new objects.
  // FIXME: Rethink and refactor vulkan_scene_graph_add_object.
  /*
  vulkan_data_object *secondObject = assetDbSceneData->objects;
  ASSERT_NEQ(secondObject, NULL);
  vulkan_scene_node *firstAddedNode =
      vulkan_scene_graph_add_object(sceneGraph, firstObjectNode, secondObject);
  vulkan_scene_graph_add_object(sceneGraph, firstObjectNode, secondObject);
  vulkan_scene_tree_debug_print(sceneGraph->sceneTree);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);
  assert_graph(sceneGraph);
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 2);
  vulkan_scene_graph_remove_object(sceneGraph, firstAddedNode);
  vulkan_scene_graph_debug_print(sceneGraph);
  assert_graph(sceneGraph);
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 0);
*/

  log_info("Test batching.");
  vulkan_batches *batches = vulkan_batches_create(renderCacheList);

  renderCacheList->dirty = false;
  vulkan_batches_update(batches, vulkan_batch_policy_none);
  vulkan_batches_debug_print(batches);
  dl_count(vulkan_batch *, batches->batches, batchNoneLen);

  vulkan_batches_update(batches, vulkan_batch_policy_matching_materials);
  vulkan_batches_debug_print(batches);
  dl_count(vulkan_batch *, batches->batches, batchMaterialsLen);
  ASSERT(batchNoneLen <= batchMaterialsLen);

  vulkan_batches_update(batches, vulkan_batch_policy_matching_vertex_attributes);
  vulkan_batches_debug_print(batches);
  dl_count(vulkan_batch *, batches->batches, batchVertexAttributesLen);
  ASSERT(batchNoneLen <= batchVertexAttributesLen);

  vulkan_batches_update(batches, vulkan_batch_policy_matching_materials |
                                     vulkan_batch_policy_matching_vertex_attributes);
  vulkan_batches_debug_print(batches);
  dl_count(vulkan_batch *, batches->batches, batchMaterialsAndVertexAttributesLen);
  ASSERT(batchNoneLen <= batchMaterialsAndVertexAttributesLen);

  vulkan_batches_destroy(batches);

  log_info("Test scene renderer.");
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_renderer *renderer = vulkan_renderer_create(config, assetDb, vks, sceneName);
  vulkan_renderer_update(renderer);
  vulkan_renderer_send_to_device(renderer);
  vulkan_renderer_debug_print(renderer);
  // HIRO more renderer tests
  ASSERT(renderer->renderState->unifiedGeometryBuffer->indexBuffer->totalSize > 0);
  ASSERT(renderer->renderState->unifiedGeometryBuffer->vertexBuffer->totalSize > 0);

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_render_cache_list_destroy(renderCacheList);
  vulkan_scene_graph_destroy(sceneGraph);
  vulkan_data_scene_destroy(assetDbSceneData);
  utstring_free(sceneName);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  PASS();
}

SUITE(scene_graph_suite) { RUN_TEST(scene_graph_building); }

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();
  platform_create();
  // RUN_SUITE(shaderc_suite);
  // RUN_SUITE(gltf_suite);
  RUN_SUITE(scene_graph_suite);
  platform_destroy();
  GREATEST_MAIN_END();
}

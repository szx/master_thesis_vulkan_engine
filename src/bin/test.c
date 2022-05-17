#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"
#include "greatest.h"

#include <stdlib.h>

#define GLTF_NAME "triangles"

// Loading sponza.gltf.
TEST gltf_loading() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  UT_string *gltfPath = get_asset_file_path("gltf/" GLTF_NAME, GLTF_NAME ".gltf");
  UT_string *sceneConfigPath = get_asset_file_path("gltf/" GLTF_NAME, GLTF_NAME ".ini");
  vulkan_scene_data *gltfSceneData =
      vulkan_scene_data_create_with_gltf_file(sceneName, gltfPath, sceneConfigPath, assetDb);
  vulkan_scene_data *assetDbSceneData = vulkan_scene_data_create_with_asset_db(assetDb, sceneName);

  ASSERT_STR_EQ(utstring_body(gltfSceneData->name), utstring_body(assetDbSceneData->name));
  ASSERT_EQ(gltfSceneData->key.value, assetDbSceneData->key.value);

#define ASSERT_VERTEX_ATTRIBUTE(_name)                                                             \
  ASSERT_EQ(utarray_len(gltfPrimitive->_name->data), utarray_len(assetDbPrimitive->_name->data));  \
  if (utarray_len(gltfPrimitive->_name->data) > 0)                                                 \
    ASSERT_MEM_EQ(utarray_front(gltfPrimitive->_name->data),                                       \
                  utarray_front(assetDbPrimitive->_name->data),                                    \
                  utarray_size(gltfPrimitive->_name->data));

  dl_foreach_elem(vulkan_asset_object *, assetDbNode, assetDbSceneData->objects) {
    bool foundCorrespondingNode = false;
    vulkan_asset_object *gltfNode;
    dl_foreach_elem(vulkan_asset_object *, nodeIter, gltfSceneData->objects) {
      if (nodeIter->key.value == assetDbNode->key.value) {
        foundCorrespondingNode = true;
        gltfNode = nodeIter;
        break;
      }
    }
    ASSERT_EQ(foundCorrespondingNode, true);
    ASSERT_MEM_EQ(gltfNode->transform, assetDbNode->transform, sizeof(mat4));
    ASSERT_EQ(utarray_len(gltfNode->children), utarray_len(assetDbNode->children));
    vulkan_asset_mesh *gltfMesh = gltfNode->mesh;
    vulkan_asset_mesh *assetDbMesh = assetDbNode->mesh;
    ASSERT((gltfMesh == NULL && assetDbMesh == NULL) ||
           (gltfMesh->key.value == assetDbMesh->key.value));
    vulkan_asset_camera *gltfCamera = gltfNode->camera;
    vulkan_asset_camera *assetDbCamera = assetDbNode->camera;
    ASSERT((gltfCamera == NULL && assetDbCamera == NULL) ||
           (gltfCamera->key.value == assetDbCamera->key.value));

    bool foundCorrespondingPrimitive = false;
    dl_foreach_elem(vulkan_asset_primitive *, gltfPrimitive, gltfSceneData->primitives) {
      vulkan_asset_primitive *assetDbPrimitive;
      dl_foreach_elem(vulkan_asset_primitive *, primitiveIter, assetDbSceneData->primitives) {
        if (gltfPrimitive->key.value == primitiveIter->key.value) {
          foundCorrespondingPrimitive = true;
          assetDbPrimitive = primitiveIter;
          break;
        }
      }
      ASSERT_EQ(foundCorrespondingPrimitive, true);
      ASSERT_EQ(gltfPrimitive->material->key.value, assetDbPrimitive->material->key.value);
      ASSERT_EQ(gltfPrimitive->topology, assetDbPrimitive->topology);
      ASSERT_EQ(gltfPrimitive->vertexCount, assetDbPrimitive->vertexCount);

      ASSERT_VERTEX_ATTRIBUTE(positions)
      ASSERT_VERTEX_ATTRIBUTE(normals)
      ASSERT_VERTEX_ATTRIBUTE(colors)
      ASSERT_VERTEX_ATTRIBUTE(texCoords)
    }
  }

  ASSERT_EQ(gltfSceneData->skybox->key.value, assetDbSceneData->skybox->key.value);
  ASSERT(strcmp(utstring_body(gltfSceneData->skybox->name),
                utstring_body(assetDbSceneData->skybox->name)) == 0);
  ASSERT_EQ(gltfSceneData->skybox->cubemapTexture->key.value,
            assetDbSceneData->skybox->cubemapTexture->key.value);

  utstring_free(gltfPath);
  utstring_free(sceneConfigPath);
  utstring_free(sceneName);
  vulkan_scene_data_destroy(assetDbSceneData);
  vulkan_scene_data_destroy(gltfSceneData);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  PASS();
}

SUITE(gltf_suite) { RUN_TEST(gltf_loading); }

TEST shaderc_compiling() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  vulkan_pipeline_type pipelines[] = {vulkan_pipeline_type_forward};
  vulkan_renderer *renderer =
      vulkan_renderer_create(config, assetDb, vks, sceneName, pipelines, array_size(pipelines));
  utstring_free(sceneName);

  vulkan_renderer_debug_print(renderer);

  vulkan_pipeline *pipeline = *(vulkan_pipeline **)utarray_front(renderer->pipelines);
  vulkan_shader *vertexShader = pipeline->shaderProgram->vertexShader;
  vulkan_shader *fragmentShader = pipeline->shaderProgram->fragmentShader;

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
  // TODO: check in vertex output variables match fragment input variables

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
  dl_foreach_elem(vulkan_scene_tree_node *, sceneNode,
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
  /* renderer cache */
  vulkan_renderer_cache *rendererCache = sceneTree->rendererCache;
  assert(dl_count(rendererCache->primitiveElements) == primitiveNodeNum);
}

// Building scene graph.
TEST scene_graph_building() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_asset);
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  vulkan_scene_data *assetDbSceneData = vulkan_scene_data_create_with_asset_db(assetDb, sceneName);
  // vulkan_scene_data_debug_print(assetDbSceneData);
  vulkan_renderer_cache *rendererCache = vulkan_renderer_cache_create(
      assetDbSceneData, config->asset.graphicsMaxPrimitiveElementCount);

  vulkan_scene_graph *sceneGraph = vulkan_scene_graph_create(assetDbSceneData, rendererCache);

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
    if (utarray_len(sceneTreeNode->childNodes) > 0) {
      sceneTreeNode = *(vulkan_scene_tree_node **)utarray_front(sceneTreeNode->childNodes);
    } else {
      sceneTreeNode = NULL;
    }
  }

  // Verify adding new objects.
  // FIXME: Rethink and refactor vulkan_scene_graph_add_object.
  /*
  vulkan_asset_object *secondObject = assetDbSceneData->objects;
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
  vulkan_batches *batches = vulkan_batches_create(vkd);

  vulkan_batches_update(batches, vulkan_batch_instancing_policy_no_instancing);
  vulkan_batches_debug_print(batches);
  size_t batchNoInstancingLen = utarray_len(batches->array);

  vulkan_batches_update(batches, vulkan_batch_instancing_policy_matching_vertex_attributes);
  vulkan_batches_debug_print(batches);
  size_t batchVertexAttributesLen = utarray_len(batches->array);
  ASSERT(batchVertexAttributesLen <= batchNoInstancingLen);

  vulkan_batches_destroy(batches);

  log_info("Test scene renderer.");
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_pipeline_type pipelines[] = {vulkan_pipeline_type_forward};
  vulkan_renderer *renderer =
      vulkan_renderer_create(config, assetDb, vks, sceneName, pipelines, array_size(pipelines));
  vulkan_renderer_update(renderer);
  vulkan_renderer_send_to_device(renderer);
  vulkan_renderer_debug_print(renderer);
  // TODO: more renderer tests
  ASSERT(renderer->renderState->unifiedGeometryBuffer->indexBuffer->totalSize > 0);
  ASSERT(renderer->renderState->unifiedGeometryBuffer->vertexBuffer->totalSize > 0);

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_renderer_cache_destroy(rendererCache);
  vulkan_scene_graph_destroy(sceneGraph);
  vulkan_scene_data_destroy(assetDbSceneData);
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
  RUN_SUITE(shaderc_suite);
  RUN_SUITE(gltf_suite);
  RUN_SUITE(scene_graph_suite);
  platform_destroy();
  GREATEST_MAIN_END();
}

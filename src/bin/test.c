#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

#include <stdlib.h>

#define GLTF_NAME "triangles"

// Loading sponza.gltf.
void gltf_loading() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_global);
  data_asset_db *assetDb = data_asset_db_create();
  device *vkd = device_create(config, assetDb);
  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  UT_string *gltfPath =
      get_file_path(globals.assetsDirname, "gltf/" GLTF_NAME "/" GLTF_NAME ".gltf");
  UT_string *sceneConfigPath =
      get_file_path(globals.assetsDirname, "gltf/" GLTF_NAME "/" GLTF_NAME ".ini");
  scene_data *gltfSceneData =
      scene_data_create_with_gltf_file(sceneName, gltfPath, sceneConfigPath, config, assetDb);
  scene_data *assetDbSceneData = scene_data_create_with_asset_db(config, assetDb, sceneName);

  verify(strcmp(utstring_body(gltfSceneData->name), utstring_body(assetDbSceneData->name)) == 0);
  verify(gltfSceneData->key.value == assetDbSceneData->key.value);

#define ASSERT_VERTEX_ATTRIBUTE(_name)                                                             \
  verify(utarray_len(gltfPrimitive->_name->data) == utarray_len(assetDbPrimitive->_name->data));   \
  if (utarray_len(gltfPrimitive->_name->data) > 0)                                                 \
    verify(core_memcmp(utarray_front(gltfPrimitive->_name->data),                                  \
                       utarray_front(assetDbPrimitive->_name->data),                               \
                       utarray_size(gltfPrimitive->_name->data)) == 0);

  dl_foreach_elem(asset_object *, assetDbNode, assetDbSceneData->objects) {
    bool foundCorrespondingNode = false;
    asset_object *gltfNode;
    dl_foreach_elem(asset_object *, nodeIter, gltfSceneData->objects) {
      if (nodeIter->key.value == assetDbNode->key.value) {
        foundCorrespondingNode = true;
        gltfNode = nodeIter;
        break;
      }
    }
    verify(foundCorrespondingNode == true);
    verify(core_memcmp(gltfNode->transform, assetDbNode->transform, sizeof(mat4)) == 0);
    verify(utarray_len(gltfNode->children) == utarray_len(assetDbNode->children));
    asset_mesh *gltfMesh = gltfNode->mesh;
    asset_mesh *assetDbMesh = assetDbNode->mesh;
    verify((gltfMesh == NULL && assetDbMesh == NULL) ||
           (gltfMesh->key.value == assetDbMesh->key.value));
    asset_camera *gltfCamera = gltfNode->camera;
    asset_camera *assetDbCamera = assetDbNode->camera;
    verify((gltfCamera == NULL && assetDbCamera == NULL) ||
           (gltfCamera->key.value == assetDbCamera->key.value));

    bool foundCorrespondingPrimitive = false;
    dl_foreach_elem(asset_primitive *, gltfPrimitive, gltfSceneData->primitives) {
      asset_primitive *assetDbPrimitive;
      dl_foreach_elem(asset_primitive *, primitiveIter, assetDbSceneData->primitives) {
        if (gltfPrimitive->key.value == primitiveIter->key.value) {
          foundCorrespondingPrimitive = true;
          assetDbPrimitive = primitiveIter;
          break;
        }
      }
      verify(foundCorrespondingPrimitive == true);
      verify(gltfPrimitive->material->key.value == assetDbPrimitive->material->key.value);
      verify(gltfPrimitive->topology == assetDbPrimitive->topology);
      verify(gltfPrimitive->vertexCount == assetDbPrimitive->vertexCount);

      ASSERT_VERTEX_ATTRIBUTE(positions)
      ASSERT_VERTEX_ATTRIBUTE(normals)
      ASSERT_VERTEX_ATTRIBUTE(colors)
      ASSERT_VERTEX_ATTRIBUTE(texCoords)
    }
  }

  verify(gltfSceneData->skybox->key.value == assetDbSceneData->skybox->key.value);
  verify(strcmp(utstring_body(gltfSceneData->skybox->name),
                utstring_body(assetDbSceneData->skybox->name)) == 0);
  verify(gltfSceneData->skybox->cubemapTexture->key.value ==
         assetDbSceneData->skybox->cubemapTexture->key.value);

  utstring_free(gltfPath);
  utstring_free(sceneConfigPath);
  utstring_free(sceneName);
  scene_data_destroy(assetDbSceneData);
  scene_data_destroy(gltfSceneData);
  device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
}

void render_pass_record_no_op(render_pass *renderPass, render_pass_frame_state *frameState,
                              VkCommandBuffer commandBuffer) {
  // No-op.
}

void shaderc_compiling() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_global);
  data_asset_db *assetDb = data_asset_db_create();
  device *vkd = device_create(config, assetDb);
  swap_chain *vks = swap_chain_create(vkd);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  renderer *renderer = renderer_create(config, assetDb, vks, sceneName);
  utstring_free(sceneName);

  render_graph_add_image_resource(renderer->renderGraph, "depthBuffer",
                                  image_type_offscreen_depth_buffer);

  render_graph_add_render_pass(
      renderer->renderGraph,
      (render_pass_desc){.vertexShader = "forward_vertex.glsl",
                         .fragmentShader = "forward_fragment.glsl",
                         .useOnscreenColorAttachment = true,
                         .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                         .offscreenColorAttachmentCount = 0,
                         .offscreenDepthAttachment =
                             {
                                 .name = "depthBuffer",
                                 .depthWriteEnable = true,
                                 .depthTestEnable = true,
                                 .depthTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                                 .clearValue = {0.0f, 0},
                             },
                         .colorBlendingType = color_blending_type_none,
                         .recordFunc = render_pass_record_no_op});
  render_graph_compile(renderer->renderGraph);
  renderer_debug_print(renderer);

  render_pass *renderPass = renderer->renderGraph->renderPassElements->renderPass;
  shader *vertexShader = renderPass->shaderProgram->vertexShader;
  shader *fragmentShader = renderPass->shaderProgram->fragmentShader;

  // verify shader
  verify(vertexShader->type == shader_type_vertex);
  verify(fragmentShader->type == shader_type_fragment);
  verify(vertexShader->spvSize > 0);
  verify(fragmentShader->spvSize > 0);
  verify(vertexShader->module != VK_NULL_HANDLE);
  verify(fragmentShader->module != VK_NULL_HANDLE);

  // verify reflection
  verify(utarray_len(vertexShader->reflect->inputVariables) > 0);
  verify(utarray_len(fragmentShader->reflect->inputVariables) > 0);
  verify(utarray_len(vertexShader->reflect->outputVariables) > 0);
  verify(utarray_len(fragmentShader->reflect->outputVariables) > 0);
  // TODO: check in vertex output variables match fragment input variables

  renderer_destroy(renderer);
  swap_chain_destroy(vks);
  device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
}

void assert_graph(scene_graph *sceneGraph) {

  scene_graph_debug_print(sceneGraph); /* scene tree */
  scene_tree *sceneTree = sceneGraph->sceneTree;
  verify(utarray_len(sceneTree->dirtyNodes) == 0);
  size_t primitiveNodeNum = 0;
  dl_foreach_elem(scene_tree_node *, sceneNode,
                  sceneTree->nodes) { /*log_debug("tree node: %p", sceneNode);*/
    if (sceneNode->object != NULL) {
      verify(utarray_len(sceneNode->childNodes) > 0 || sceneNode->object->camera != NULL);
    }
    if (sceneNode->primitive != NULL) {
      verify(utarray_len(sceneNode->childNodes) == 0);
      primitiveNodeNum++;
    }
    verify(!sceneNode->dirty);
  }
  /* renderer cache */
  renderer_cache *rendererCache = sceneTree->rendererCache;
  size_t primitiveElementNum = 0;
  dl_foreach_elem(renderer_cache_primitive_element *, primitiveElement,
                  rendererCache->primitiveElements) {
    if (primitiveElement->sourceType == renderer_cache_primitive_element_source_type_scene_tree) {
      primitiveElementNum++;
    }
  }
  verify(primitiveElementNum == primitiveNodeNum);
}

// Building scene graph.
void scene_graph_building() {
  data_config *config = data_config_create(globals.assetConfigFilepath, data_config_type_global);
  data_asset_db *assetDb = data_asset_db_create();
  device *vkd = device_create(config, assetDb);

  UT_string *sceneName;
  utstring_alloc(sceneName, GLTF_NAME);
  scene_data *assetDbSceneData = scene_data_create_with_asset_db(config, assetDb, sceneName);
  // scene_data_debug_print(assetDbSceneData);
  renderer_cache *rendererCache =
      renderer_cache_create(assetDbSceneData, config->global.graphicsMaxPrimitiveElementCount);

  scene_graph *sceneGraph = scene_graph_create(assetDbSceneData, rendererCache);

  log_info("Verify cache accumulation.");
  scene_graph_node *firstObjectNode =
      *(scene_graph_node **)utarray_front(sceneGraph->root->childNodes);
  verify(firstObjectNode != NULL);
  verify(firstObjectNode->object != NULL);
  firstObjectNode->object->transform[0][0] = 2;
  scene_graph_set_dirty(sceneGraph, firstObjectNode);
  scene_tree_validate(sceneGraph->sceneTree);
  assert_graph(sceneGraph);

  scene_tree_node *sceneTreeNode = *(scene_tree_node **)utarray_front(firstObjectNode->observers);
  while (sceneTreeNode != NULL) {
    if (utarray_len(sceneTreeNode->childNodes) > 0) {
      sceneTreeNode = *(scene_tree_node **)utarray_front(sceneTreeNode->childNodes);
    } else {
      sceneTreeNode = NULL;
    }
  }

  // Verify adding new objects.
  // FIXME: Rethink and refactor scene_graph_add_object.
  /*
  asset_object *secondObject = assetDbSceneData->objects;
  ASSERT_NEQ(secondObject, NULL);
  scene_node *firstAddedNode =
      scene_graph_add_object(sceneGraph, firstObjectNode, secondObject);
  scene_graph_add_object(sceneGraph, firstObjectNode, secondObject);
  scene_tree_debug_print(sceneGraph->sceneTree);
  scene_tree_validate(sceneGraph->sceneTree);
  assert_graph(sceneGraph);
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 2);
  scene_graph_remove_object(sceneGraph, firstAddedNode);
  scene_graph_debug_print(sceneGraph);
  assert_graph(sceneGraph);
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 0);
*/

  log_info("Test batching.");
  batches *batches = batches_create(vkd);

  batches_update(batches, batch_instancing_policy_no_instancing);
  batches_debug_print(batches);
  size_t batchNoInstancingLen = utarray_len(batches->array);

  batches_update(batches, batch_instancing_policy_matching_vertex_attributes);
  batches_debug_print(batches);
  size_t batchVertexAttributesLen = utarray_len(batches->array);
  verify(batchVertexAttributesLen <= batchNoInstancingLen);

  batches_destroy(batches);

  log_info("Test scene renderer.");
  swap_chain *vks = swap_chain_create(vkd);
  renderer *renderer = renderer_create(config, assetDb, vks, sceneName);
  render_graph_add_image_resource(renderer->renderGraph, "depthBuffer",
                                  image_type_offscreen_depth_buffer);
  render_graph_add_render_pass(
      renderer->renderGraph,
      (render_pass_desc){.vertexShader = "forward_vertex.glsl",
                         .fragmentShader = "forward_fragment.glsl",
                         .useOnscreenColorAttachment = true,
                         .onscreenClearValue = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}},
                         .offscreenColorAttachmentCount = 0,
                         .offscreenDepthAttachment =
                             {
                                 .name = "depthBuffer",
                                 .depthWriteEnable = true,
                                 .depthTestEnable = true,
                                 .depthTestOp = VK_COMPARE_OP_GREATER_OR_EQUAL,
                                 .clearValue = {0.0f, 0},
                             },
                         .colorBlendingType = color_blending_type_none,
                         .recordFunc = render_pass_record_no_op});

  renderer_update(renderer);
  renderer_send_to_device(renderer);
  renderer_debug_print(renderer);
  // TODO: more renderer tests
  verify(renderer->renderState->unifiedGeometryBuffer->indexBuffer->totalSize > 0);
  verify(renderer->renderState->unifiedGeometryBuffer->vertexBuffer->totalSize > 0);

  renderer_destroy(renderer);
  swap_chain_destroy(vks);
  renderer_cache_destroy(rendererCache);
  scene_graph_destroy(sceneGraph);
  scene_data_destroy(assetDbSceneData);
  utstring_free(sceneName);
  device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
}

int main(int argc, char *argv[]) {
  platform_create(argc, argv);
  log_info("starting tests");

  shaderc_compiling();
  gltf_loading();
  scene_graph_building();

  log_info("finished tests");
  platform_destroy();
  return 0;
}

#include "../core/core.h"
#include "../data/data.h"
#include "../parser/glsl_parser.h"
#include "../vulkan/vulkan.h"
#include "greatest.h"

#include <stdlib.h>

#define GLTF_NAME "sponza"

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
  ASSERT_EQ(utarray_len(gltfSceneData->cameras), utarray_len(assetDbSceneData->cameras));

  utarray_foreach_elem_it (vulkan_data_camera *, assetDbCamera, assetDbSceneData->cameras) {
    bool foundCorrespondingCamera = false;
    utarray_foreach_elem_it (vulkan_data_camera *, gltfCamera, gltfSceneData->cameras) {
      if (gltfCamera->key.value == assetDbCamera->key.value) {
        foundCorrespondingCamera = true;
        break;
      }
    }
    ASSERT_EQ(foundCorrespondingCamera, true);
    ASSERT_MEM_EQ(gltfCamera->position, assetDbCamera->position, sizeof(vec3));
    ASSERT_MEM_EQ(gltfCamera->rotation, assetDbCamera->rotation, sizeof(versor));
    ASSERT_EQ(gltfCamera->fovY, assetDbCamera->fovY);
    ASSERT_EQ(gltfCamera->aspectRatio, assetDbCamera->aspectRatio);
    ASSERT_EQ(gltfCamera->nearZ, assetDbCamera->nearZ);
    ASSERT_EQ(gltfCamera->farZ, assetDbCamera->farZ);
  }

  dl_foreach_elem (vulkan_data_object *, assetDbNode, assetDbSceneData->objects) {
    bool foundCorrespondingNode = false;
    dl_foreach_elem (vulkan_data_object *, gltfNode, gltfSceneData->objects) {
      if (gltfNode->key.value == assetDbNode->key.value) {
        foundCorrespondingNode = true;
        break;
      }
    }
    ASSERT_EQ(foundCorrespondingNode, true);
    ASSERT_MEM_EQ(gltfNode->transform, assetDbNode->transform, sizeof(mat4));
    ASSERT_EQ(utarray_len(gltfNode->children), utarray_len(assetDbNode->children));
    vulkan_data_mesh *gltfMesh = gltfNode->mesh;
    vulkan_data_mesh *assetDbMesh = assetDbNode->mesh;
    ASSERT((gltfMesh == NULL && assetDbMesh == NULL) ||
           (gltfMesh->key.value == assetDbMesh->key.value));

    bool foundCorrespondingPrimitive = false;
    dl_foreach_elem (vulkan_data_primitive *, gltfPrimitive, gltfSceneData->primitives) {
      dl_foreach_elem (vulkan_data_primitive *, assetDbPrimitive, assetDbSceneData->primitives) {
        if (gltfPrimitive->key.value == assetDbPrimitive->key.value) {
          foundCorrespondingPrimitive = true;
          break;
        }
      }
      ASSERT_EQ(foundCorrespondingPrimitive, true);
      ASSERT_EQ(gltfPrimitive->material->key.value, assetDbPrimitive->material->key.value);
      ASSERT_EQ(gltfPrimitive->topology, assetDbPrimitive->topology);
      ASSERT_EQ(gltfPrimitive->vertexCount, assetDbPrimitive->vertexCount);
#define ASSERT_VERTEX_ATTRIBUTE(_name)                                                             \
  ASSERT_EQ(utarray_len(gltfPrimitive->_name->data), utarray_len(assetDbPrimitive->_name->data));  \
  if (utarray_len(gltfPrimitive->_name->data) > 0)                                                 \
    ASSERT_MEM_EQ(utarray_front(gltfPrimitive->_name->data),                                       \
                  utarray_front(assetDbPrimitive->_name->data),                                    \
                  utarray_size(gltfPrimitive->_name->data));
      ASSERT_VERTEX_ATTRIBUTE(positions)
      ASSERT_VERTEX_ATTRIBUTE(normals)
      ASSERT_VERTEX_ATTRIBUTE(colors)
      ASSERT_VERTEX_ATTRIBUTE(texCoords)
    }
  }
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

  UT_string *vertInputPath = get_asset_file_path("shaders", "shader.vert");
  UT_string *fragInputPath = get_asset_file_path("shaders", "shader.frag");
  vulkan_shader *vertShader = vulkan_shader_create_with_path(vkd, vertInputPath);
  vulkan_shader *fragShader = vulkan_shader_create_with_path(vkd, fragInputPath);
  utstring_free(vertInputPath);
  utstring_free(fragInputPath);

  ASSERT_EQ(vertShader->type, shaderc_glsl_vertex_shader);
  ASSERT_EQ(fragShader->type, shaderc_glsl_fragment_shader);
  log_debug("maxVertexInputAttributes=%d", vkd->limits.maxVertexInputAttributes);
  log_debug("maxVertexOutputComponents/4=%d", vkd->limits.maxVertexOutputComponents / 4);
  size_t inputAttributeCount = core_array_count(vertShader->info.inputAttributeDescriptions);
  size_t outputAttributeCount = core_array_count(vertShader->info.outputAttributeDescriptions);
  ASSERT_GT(inputAttributeCount, 0);
  ASSERT_GT(outputAttributeCount, 0);

  log_debug("inputAttributeDescriptionsSize=%d", inputAttributeCount);
  log_debug("outputAttributeDescriptionsSize=%d", outputAttributeCount);
  verify(inputAttributeCount < vkd->limits.maxVertexInputAttributes);
  verify(outputAttributeCount < vkd->limits.maxVertexOutputComponents / 4);

  VkVertexInputBindingDescription bindingDescription =
      vulkan_shader_info_get_binding_description(&vertShader->info);
  size_t attributeDescriptionsCount;
  VkVertexInputAttributeDescription *attributeDescriptions =
      vulkan_shader_info_get_attribute_descriptions(&vertShader->info, &attributeDescriptionsCount);
  VkPushConstantRange range = vulkan_shader_info_get_push_constant_range(vertShader, fragShader);
  ASSERT_EQ(range.offset, 0);
  ASSERT_GT(range.size, 0);
  ASSERT((range.stageFlags | VK_SHADER_STAGE_VERTEX_BIT) != 0 ||
         (range.stageFlags | VK_SHADER_STAGE_FRAGMENT_BIT) != 0);

  data_config_destroy(config);
  data_asset_db_destroy(assetDb);
  vulkan_shader_destroy(vertShader);
  vulkan_shader_destroy(fragShader);
  vulkan_device_destroy(vkd);
  PASS();
}

SUITE(shaderc_suite) { RUN_TEST(shaderc_compiling); }

// Building scene graph for sponza.
TEST scene_graph_building() {
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);

  UT_string *sceneName;
  utstring_alloc(sceneName, "sponza");
  vulkan_data_scene *assetDbSceneData = vulkan_data_scene_create_with_asset_db(assetDb, sceneName);
  utstring_free(sceneName);
  // vulkan_data_scene_debug_print(assetDbSceneData);

  vulkan_scene_graph *sceneGraph = vulkan_scene_graph_create(assetDbSceneData);

#define ASSERT_GRAPH()                                                                             \
  do {                                                                                             \
    vulkan_scene_graph_debug_print(sceneGraph);                                                    \
    /* scene tree */                                                                               \
    vulkan_scene_tree *sceneTree = sceneGraph->sceneTree;                                          \
    ASSERT_EQ(utarray_len(sceneTree->dirtyNodes), 0);                                              \
    size_t primitiveNodeNum = 0;                                                                   \
    dl_foreach_elem (vulkan_scene_node *, sceneNode, sceneTree->nodes) {                           \
      /*log_debug("tree node: %p", sceneNode);*/                                                   \
      ASSERT_EQ(sceneNode->containerType, vulkan_scene_node_container_type_scene_tree);            \
      ASSERT(sceneNode->type == vulkan_scene_node_entity_type_root ||                              \
             utarray_len(sceneNode->parentNodes) > 0);                                             \
      if (sceneNode->type == vulkan_scene_node_entity_type_root) {                                 \
        ASSERT_GT(utarray_len(sceneNode->childObjectNodes), 0);                                    \
        ASSERT_EQ(utarray_len(sceneNode->primitiveNodes), 0);                                      \
      } else if (sceneNode->type == vulkan_scene_node_entity_type_object) {                        \
        ASSERT(utarray_len(sceneNode->primitiveNodes) > 0 ||                                       \
               utarray_len(sceneNode->childObjectNodes) > 0);                                      \
      } else if (sceneNode->type == vulkan_scene_node_entity_type_primitive) {                     \
        ASSERT_EQ(utarray_len(sceneNode->childObjectNodes), 0);                                    \
        ASSERT_EQ(utarray_len(sceneNode->primitiveNodes), 0);                                      \
        primitiveNodeNum++;                                                                        \
      } else {                                                                                     \
        FAILm("unknown node type");                                                                \
      }                                                                                            \
      ASSERT_FALSE(sceneNode->dirty);                                                              \
    }                                                                                              \
    /* cache list */                                                                               \
    vulkan_scene_cache_list *cacheList = sceneTree->cacheList;                                     \
    ASSERT(utarray_len(cacheList->caches) == primitiveNodeNum);                                    \
  } while (0)

  ASSERT_GRAPH();

  log_info("Verify cache accumulation.");
  vulkan_scene_node *firstObjectNode =
      *(vulkan_scene_node **)utarray_front(sceneGraph->root->childObjectNodes);
  ASSERT_NEQ(firstObjectNode, NULL);
  ASSERT_EQ(firstObjectNode->type, vulkan_scene_node_entity_type_object);
  firstObjectNode->object->transform[0][0] = 2;
  vulkan_scene_node_set_dirty(firstObjectNode);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);
  ASSERT_GRAPH();

  vulkan_scene_node *sceneTreeNode =
      *(vulkan_scene_node **)utarray_front(firstObjectNode->observers);
  while (sceneTreeNode != NULL) {
    ASSERT_EQ(sceneTreeNode->cache->transform[0][0], 2);
    if (utarray_len(sceneTreeNode->childObjectNodes) > 0) {
      sceneTreeNode = *(vulkan_scene_node **)utarray_front(sceneTreeNode->childObjectNodes);
    } else if (utarray_len(sceneTreeNode->primitiveNodes) > 0) {
      sceneTreeNode = *(vulkan_scene_node **)utarray_front(sceneTreeNode->primitiveNodes);
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
  ASSERT_GRAPH();
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 2);
  vulkan_scene_graph_remove_object(sceneGraph, firstAddedNode);
  vulkan_scene_graph_debug_print(sceneGraph);
  ASSERT_GRAPH();
  ASSERT_EQ(utarray_len(firstObjectNode->childObjectNodes), 0);
*/

  log_info("Test batching.");
  vulkan_batches *batches = vulkan_batches_create(sceneGraph);

  vulkan_batches_update(batches, vulkan_batch_policy_none);
  vulkan_batches_debug_print(batches);
  // HIRO DL_COUNT macro
  vulkan_batches_update(batches, vulkan_batch_policy_matching_materials);
  vulkan_batches_debug_print(batches);

  vulkan_batches_update(batches, vulkan_batch_policy_matching_vertex_attributes);
  vulkan_batches_debug_print(batches);

  vulkan_batches_update(batches, vulkan_batch_policy_matching_materials |
                                     vulkan_batch_policy_matching_vertex_attributes);
  vulkan_batches_debug_print(batches);

  vulkan_batches_destroy(batches);

  vulkan_scene_graph_destroy(sceneGraph);
  vulkan_data_scene_destroy(assetDbSceneData);
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

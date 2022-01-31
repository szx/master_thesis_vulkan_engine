#include "../core/core.h"
#include "../data/data.h"
#include "../parser/glsl_parser.h"
#include "../vulkan/vulkan.h"
#include "greatest.h"

#include <stdlib.h>

// Loading sponza.gltf.
TEST gltf_loading() {
  // TODO: Loading extra files (images).
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  UT_string *sceneName;
  utstring_alloc(sceneName, "sponza");
  UT_string *gltfPath = get_asset_file_path("sponza", "sponza.gltf");
  vulkan_scene_data *gltfSceneData = vulkan_scene_data_create_with_gltf_file(sceneName, gltfPath);
  vulkan_scene_data *assetDbSceneData = vulkan_scene_data_create_with_asset_db(assetDb, sceneName);

  ASSERT_STR_EQ(utstring_body(gltfSceneData->name), utstring_body(assetDbSceneData->name));
  ASSERT_EQ(gltfSceneData->dirty, assetDbSceneData->dirty);
  ASSERT_EQ(utarray_len(gltfSceneData->nodes), utarray_len(assetDbSceneData->nodes));
  ASSERT_EQ(utarray_len(gltfSceneData->primitives), utarray_len(assetDbSceneData->primitives));
  vulkan_node_data *assetDbNode = NULL;
  while ((assetDbNode = (utarray_next(assetDbSceneData->nodes, assetDbNode)))) {
    vulkan_node_data *gltfNode = NULL;
    bool foundCorrespondingNode = false;
    while ((gltfNode = (utarray_next(gltfSceneData->nodes, gltfNode)))) {
      if (gltfNode->hash == assetDbNode->hash) {
        foundCorrespondingNode = true;
        break;
      }
    }
    ASSERT_EQ(foundCorrespondingNode, true);
    ASSERT_MEM_EQ(gltfNode->transform, assetDbNode->transform, sizeof(mat4));
    vulkan_mesh_data *gltfMesh = &gltfNode->mesh;
    vulkan_mesh_data *assetDbMesh = &assetDbNode->mesh;
    ASSERT_EQ(gltfMesh->hash, assetDbMesh->hash);
    vulkan_primitive_data *gltfPrimitive = NULL;
    vulkan_primitive_data *assetDbPrimitive = NULL;
    bool foundCorrespondingPrimitive = false;
    vulkan_primitive_data_index *gltfPrimIdx = NULL;
    while ((gltfPrimIdx = (utarray_next(gltfMesh->primitives, gltfPrimIdx)))) {
      gltfPrimitive = utarray_eltptr(gltfSceneData->primitives, *gltfPrimIdx);
      vulkan_primitive_data_index *assetDbPrimIdx = NULL;
      while ((assetDbPrimIdx = (utarray_next(gltfMesh->primitives, assetDbPrimIdx)))) {
        assetDbPrimitive = utarray_eltptr(assetDbSceneData->primitives, *assetDbPrimIdx);
        if (gltfPrimitive->hash == assetDbPrimitive->hash) {
          foundCorrespondingPrimitive = true;
          break;
        }
      }
    }
    ASSERT_EQ(foundCorrespondingPrimitive, true);
    ASSERT_EQ(gltfPrimitive->topology, assetDbPrimitive->topology);
    ASSERT_EQ(gltfPrimitive->vertexCount, assetDbPrimitive->vertexCount);
#define ASSERT_VERTEX_ATTRIBUTE(_name)                                                             \
  ASSERT_EQ(utarray_len(gltfPrimitive->_name), utarray_len(assetDbPrimitive->_name));              \
  if (utarray_len(gltfPrimitive->_name) > 0)                                                       \
    ASSERT_MEM_EQ(utarray_front(gltfPrimitive->_name), utarray_front(assetDbPrimitive->_name),     \
                  utarray_size(gltfPrimitive->_name));
    ASSERT_VERTEX_ATTRIBUTE(positions)
    ASSERT_VERTEX_ATTRIBUTE(normals)
    ASSERT_VERTEX_ATTRIBUTE(colors)
    ASSERT_VERTEX_ATTRIBUTE(texCoords)
  }
  utstring_free(gltfPath);
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

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();
  platform_create();
  RUN_SUITE(gltf_suite);
  // RUN_SUITE(shaderc_suite);
  platform_destroy();
  GREATEST_MAIN_END();
}

#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

typedef struct asset_pipeline_input {
  UT_string *sourceAssetType;
  UT_string *sourceAssetName;
  UT_string *sourceAssetPath;
} asset_pipeline_input;

asset_pipeline_input *asset_pipeline_input_create() {
  asset_pipeline_input *inputs = core_alloc(sizeof(asset_pipeline_input));
  utstring_new(inputs->sourceAssetType);
  utstring_new(inputs->sourceAssetName);
  utstring_new(inputs->sourceAssetPath);
  return inputs;
}

void asset_pipeline_input_destroy(asset_pipeline_input *input) {
  utstring_free(input->sourceAssetType);
  utstring_free(input->sourceAssetName);
  utstring_free(input->sourceAssetPath);
  core_free(input);
}

void parse_arguments(int argc, char *argv[], asset_pipeline_input **assetInput) {
  verify(argc == 2 || argc == 4);
  *assetInput = asset_pipeline_input_create();
  utstring_printf((*assetInput)->sourceAssetType, "%s", argv[1]);
  if (argc == 4) {
    utstring_printf((*assetInput)->sourceAssetName, "%s", argv[2]);
    utstring_printf((*assetInput)->sourceAssetPath, "%s", argv[3]);
  }
}

void write_default_config() {
  data_config *config = data_config_create();
  data_config_set_default_values(config);
  data_config_save(config);
  data_config_destroy(config);
}

void write_meshes_to_assets(data_assets *assets, asset_pipeline_input *assetInput) {
  log_info("processing gltf '%s' in '%s'", utstring_body(assetInput->sourceAssetName),
           utstring_body(assetInput->sourceAssetPath));
  vulkan_scene_data *data = vulkan_scene_data_create_with_gltf_file(assetInput->sourceAssetPath);
  for (size_t nodeIdx = 0; nodeIdx < core_array_count(data->nodes); nodeIdx++) {
    vulkan_node *node = &data->nodes.ptr[nodeIdx];
    log_debug("node %zu", nodeIdx);
    glm_mat4_print(node->modelMat, stderr);
    vulkan_mesh *mesh = &node->mesh;
    UT_string *meshName;
    utstring_new(meshName);
    for (size_t primIdx = 0; primIdx < core_array_count(mesh->primitives); primIdx++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives.ptr[primIdx];
      log_debug("  primitive %d: %s\n", primIdx,
                VkPrimitiveTopology_debug_str(primitive->topology));
      utstring_clear(meshName); // HIRO hash key
      utstring_printf(meshName, "%s_%zu", utstring_body(assetInput->sourceAssetName), primIdx);
      data_assets_insert_primitive_int(assets, utstring_body(meshName), "topology",
                                       primitive->topology);
      data_assets_insert_primitive_blob(
          assets, utstring_body(meshName), "indices", utarray_front(primitive->indexBuffer),
          primitive->indexBuffer->icd.sz * utarray_len(primitive->indexBuffer));
      data_assets_insert_primitive_blob(
          assets, utstring_body(meshName), "vertices", utarray_front(primitive->vertexStream),
          primitive->vertexStream->icd.sz * utarray_len(primitive->vertexStream));
      data_assets_insert_primitive_int(assets, utstring_body(meshName), "vertexAttributes",
                                       primitive->vertexAttributes);
    }
    // HIRO add actual scene
    utstring_free(meshName);
  }
  vulkan_scene_data_destroy(data);
}

int main(int argc, char *argv[]) {
  platform_create();
  log_info("parsing arguments");
  asset_pipeline_input *input = NULL;
  parse_arguments(argc, argv, &input);
  log_info("source asset type: '%s'", utstring_body(input->sourceAssetType));
  if (strcmp("empty_config", utstring_body(input->sourceAssetType)) == 0) {
    log_info("write default config");
    write_default_config();
  } else if (strcmp("empty_assets", utstring_body(input->sourceAssetType)) == 0) {
    log_info("open SQLite asset database");
    data_assets *assets = data_assets_create();
    log_info("save empty asset database");
    data_assets_save_empty(assets);
    log_info("close SQLite asset database");
    data_assets_destroy(assets);
  } else if (strcmp("gltf", utstring_body(input->sourceAssetType)) == 0) {
    log_info("open SQLite asset database");
    data_assets *assets = data_assets_create();
    log_info("write meshes to asset database");
    write_meshes_to_assets(assets, input);
    log_info("close SQLite asset database");
    data_assets_destroy(assets);
  }
  log_info("finished asset pipeline");
  asset_pipeline_input_destroy(input);
  platform_destroy();
  return 0;
}

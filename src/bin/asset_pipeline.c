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

void write_meshes_to_assets(data_asset_db *assetDb, asset_pipeline_input *assetInput) {
  log_info("processing gltf '%s' in '%s'", utstring_body(assetInput->sourceAssetName),
           utstring_body(assetInput->sourceAssetPath));
  vulkan_scene_data *sceneData = vulkan_scene_data_create_with_gltf_file(
      assetInput->sourceAssetName, assetInput->sourceAssetPath);

  UT_array *cameraKeys = NULL;
  utarray_alloc(cameraKeys, sizeof(data_key));
  vulkan_camera_data *camera = NULL;
  while ((camera = (utarray_next(sceneData->cameras, camera)))) {
    vulkan_camera_data_serialize(camera, assetDb);
    utarray_push_back(cameraKeys, &camera->hash);
  }
  data_asset_db_insert_scene_cameras_key_array(assetDb, sceneData->hash,
                                               data_key_array_temp(cameraKeys));

  UT_array *nodeHashes = NULL;
  utarray_alloc(nodeHashes, sizeof(data_key));
  vulkan_node_data *node = NULL;
  while ((node = (utarray_next(sceneData->nodes, node)))) {
    // vulkan_node_data_debug_print(node);

    vulkan_mesh_data *mesh = &node->mesh;
    UT_array *primitiveHashes = NULL;
    utarray_alloc(primitiveHashes, sizeof(data_key));

    vulkan_primitive_data_index *primitiveIdx = NULL;
    while ((primitiveIdx = (utarray_next(mesh->primitives, primitiveIdx)))) {
      vulkan_primitive_data *primitive = utarray_eltptr(sceneData->primitives, *primitiveIdx);
      vulkan_primitive_data_serialize(primitive, assetDb);
      utarray_push_back(primitiveHashes, &primitive->hash);
    }

    data_asset_db_insert_mesh_primitives_key_array(assetDb, mesh->hash,
                                                   data_key_array_temp(primitiveHashes));
    utarray_free(primitiveHashes);

    data_mat4 transformMat;
    glm_mat4_copy(node->transform, transformMat.value);
    data_asset_db_insert_node_transform_mat4(assetDb, node->hash, transformMat);

    data_asset_db_insert_node_mesh_key(assetDb, node->hash, node->mesh.hash);
    utarray_push_back(nodeHashes, &node->hash);
    // TODO: child nodes
  }
  data_asset_db_insert_scene_name_text(assetDb, sceneData->hash, (data_text){sceneData->name});
  data_asset_db_insert_scene_nodes_key_array(assetDb, sceneData->hash,
                                             data_key_array_temp(nodeHashes));

  // TODO: Add textures to pipeline.

  utarray_free(nodeHashes);
  vulkan_scene_data_destroy(sceneData);
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
    data_asset_db *assetDb = data_asset_db_create();
    log_info("save empty asset database");
    data_asset_db_save_empty(assetDb);
    log_info("close SQLite asset database");
    data_asset_db_destroy(assetDb);
  } else if (strcmp("gltf", utstring_body(input->sourceAssetType)) == 0) {
    log_info("open SQLite asset database");
    data_asset_db *assetDb = data_asset_db_create();
    log_info("write meshes to asset database");
    write_meshes_to_assets(assetDb, input);
    log_info("close SQLite asset database");
    data_asset_db_destroy(assetDb);
  }
  log_info("finished asset pipeline");
  asset_pipeline_input_destroy(input);
  platform_destroy();
  return 0;
}

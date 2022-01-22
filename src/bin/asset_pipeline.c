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
  UT_array *nodeHashes = NULL;
  utarray_alloc(nodeHashes, sizeof(hash_t));
  for (size_t nodeIdx = 0; nodeIdx < core_array_count(sceneData->nodes); nodeIdx++) {
    vulkan_node *node = &sceneData->nodes.ptr[nodeIdx];
    // vulkan_node_debug_print(node);

    vulkan_mesh *mesh = &node->mesh;
    UT_array *primitiveHashes = NULL;
    utarray_alloc(primitiveHashes, sizeof(hash_t));

    for (size_t primIdx = 0; primIdx < core_array_count(mesh->primitives); primIdx++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives.ptr[primIdx];

      data_asset_db_insert_primitive_topology_int(assetDb, hash_blob(primitive->hash),
                                                  primitive->topology);
      data_asset_db_insert_primitive_indices_blob(assetDb, hash_blob(primitive->hash),
                                                  utarray_blob(primitive->indices));
      data_asset_db_insert_primitive_positions_blob(assetDb, hash_blob(primitive->hash),
                                                    utarray_blob(primitive->positions));
      data_asset_db_insert_primitive_normals_blob(assetDb, hash_blob(primitive->hash),
                                                  utarray_blob(primitive->normals));
      data_asset_db_insert_primitive_colors_blob(assetDb, hash_blob(primitive->hash),
                                                 utarray_blob(primitive->colors));
      data_asset_db_insert_primitive_tex_coords_blob(assetDb, hash_blob(primitive->hash),
                                                     utarray_blob(primitive->texCoords));

      utarray_push_back(primitiveHashes, &primitive->hash);
    }

    data_asset_db_insert_mesh_primitives_hash_array(assetDb, hash_blob(mesh->hash),
                                                    utarray_hash_array(primitiveHashes));
    utarray_free(primitiveHashes);

    data_asset_db_insert_node_transform_blob(assetDb, hash_blob(node->hash),
                                             (data_blob){node->modelMat, sizeof(node->modelMat)});
    data_asset_db_insert_node_mesh_hash(assetDb, hash_blob(node->hash), node->mesh.hash);
    utarray_push_back(nodeHashes, &node->hash);
    // TODO: child nodes
  }
  data_asset_db_insert_scene_nodes_hash_array(assetDb, utstring_blob(sceneData->name),
                                              utarray_hash_array(nodeHashes));

  data_blob cameraBlob = vulkan_camera_serialize(sceneData->camera);
  data_asset_db_insert_scene_cameras_blob(assetDb, utstring_blob(sceneData->name), cameraBlob);
  core_free(cameraBlob.memory);

  // TODO: Add textures to pipeline.

  {
    // Quick sanity check.
    UT_string *name = data_asset_db_select_scene_key_text(assetDb, utstring_blob(sceneData->name));
    assert(strcmp(utstring_body(name), utstring_body(sceneData->name)) == 0);
    utstring_free(name);
  }

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

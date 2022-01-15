#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

typedef struct gltf_input {
  UT_string *gltfName;
  UT_string *gltfPath;
} gltf_input;
gltf_input *gltf_input_array_create(size_t count);
void gltf_input_array_destroy(gltf_input *inputs, size_t count);

void parse_arguments(int argc, char *argv[], size_t *gltfInputsCount, gltf_input *(*gltfInputs));
void write_default_config();
void write_meshes_to_assets(data_assets *assets, gltf_input *gltfInputs, size_t gltfInputsCount);

int main(int argc, char *argv[]) {
  platform_create();
  // HIRO extract
  log_info("parsing arguments");
  size_t gltfInputsCount;
  gltf_input *gltfInputs;
  parse_arguments(argc, argv, &gltfInputsCount, &gltfInputs);
  log_info("write default config");
  write_default_config();
  log_info("open SQLite asset database");
  data_assets *assets = data_assets_create();
  log_info("save empty asset database");
  data_assets_save_empty(assets);
  log_info("write meshes to asset database");
  write_meshes_to_assets(assets, gltfInputs, gltfInputsCount);
  gltf_input_array_destroy(gltfInputs, gltfInputsCount);
  log_info("finished asset pipeline");
  data_assets_destroy(assets);
  platform_destroy();
  return 0;
}

gltf_input *gltf_input_array_create(size_t count) {
  gltf_input *inputs = core_alloc(count * sizeof(gltf_input));
  return inputs;
}

void gltf_input_array_destroy(gltf_input *inputs, size_t count) {
  for (size_t i = 0; i < count; i++) {
    utstring_free(inputs[i].gltfName);
    utstring_free(inputs[i].gltfPath);
  }
  core_free(inputs);
}

void parse_arguments(int argc, char *argv[], size_t *gltfInputsCount, gltf_input *(*gltfInputs)) {
  verify(argc > 1);
  log_info("arguments count = %d", argc - 1);
  *gltfInputsCount = atoi(argv[1]);
  verify((argc - 2) % *gltfInputsCount == 0);
  log_info("gltf inputs count = %zu", *gltfInputsCount);
  *gltfInputs = gltf_input_array_create(*gltfInputsCount);
  for (size_t i = 0; i < *gltfInputsCount; i++) {
    UT_string *gltfName, *gltfPath;
    utstring_new(gltfName);
    utstring_new(gltfPath);
    utstring_printf(gltfName, "%s", argv[i + 2]);
    utstring_printf(gltfPath, "%s", argv[i + 2 + *gltfInputsCount]);
    (*gltfInputs)[i].gltfName = gltfName;
    (*gltfInputs)[i].gltfPath = gltfPath;
  }
}

void write_default_config() {
  data_config *config = data_config_create();

  config->dirty = true;
  data_config_set_int(config, "graphics", "WindowWidth", 800);
  data_config_set_int(config, "graphics", "WindowHeight", 600);
  data_config_set_str(config, "graphics", "WindowTitle", "cpptest");
  data_config_set_int(config, "controls", "Enabled", 1);

  data_config_save(config);
  data_config_destroy(config);
}

void write_meshes_to_assets(data_assets *assets, gltf_input *gltfInputs, size_t gltfInputsCount) {
  for (size_t i = 0; i < gltfInputsCount; i++) {
    gltf_input gltfInput = gltfInputs[i];
    log_info("processing gltf '%s' in '%s", utstring_body(gltfInput.gltfName),
             utstring_body(gltfInput.gltfPath));
    // vulkan_scene *scene = vulkan_scene_create_with_gltf_file(vkd, gltfPath);
    //  HIRO separate gltf.h from scene.h
    //  HIRO parse scene
  }
}

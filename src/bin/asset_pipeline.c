#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void write_default_config();
void write_meshes_to_assets(data_assets *assets);

int main(int argc, char *argv[]) {
  platform_create();
  log_info("write default config");
  write_default_config();
  log_info("open SQLite asset database");
  data_assets *assets = data_assets_create();
  log_info("save empty asset database");
  data_assets_save_empty(assets);
  log_info("write meshes to asset database");
  write_meshes_to_assets(assets);
  log_info("finished asset pipeline");
  data_assets_destroy(assets);
  platform_destroy();
  return 0;
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

void write_meshes_to_assets(data_assets *assets) {
  // TODO: Write meshes to asset database.
}

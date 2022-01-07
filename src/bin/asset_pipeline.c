#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_init();
  log_info("create default SQLite database");
  data_assets *assets = data_assets_create(false);
  log_info("write default config");
  assets->windowWidth = 800;
  assets->windowHeight = 600;
  utstring_printf(assets->windowTitle, "cpptest");
  // HIRO ^config file (maybe key-value pairs in argv? Lua?)
  log_info("save asset pipeline");
  data_assets_destroy(assets);
  log_info("finished asset pipeline");
  platform_free();
  return 0;
}

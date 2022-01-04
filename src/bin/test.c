#include "../core/platform.h"
#include "../peg/c_parser.h"
#include "../peg/glsl_parser.h"
#include "../vulkan/vulkan.h"
#include "greatest.h"

#include <stdlib.h>

// Parsing C preprocessor directives.
TEST c_parser_preprocessor_parsing() {
  size_t inputSize;
  platform_path inputPath = get_executable_dir_path();
  platform_path_append(&inputPath, "/tests/c_parser_test.txt");
  char *input = read_text_file(&inputPath, &inputSize);
  platform_path_deinit(&inputPath);
  if (input == NULL) {
    FAILm("failed to load file");
  }
  parser_state state = c_parser_execute(input);
  parser_debug_print(&state);
  parser_state_free(&state);
  free(input);
  PASS();
}

SUITE(c_parser_suite) { RUN_TEST(c_parser_preprocessor_parsing); }

#include "../data/config.h"
#include <sqlite3.h>

int database_resolutions_callback(void *callbackData, int argc, char **argv, char **azColName) {
  for (int i = 0; i < argc; i++) {
    log_debug("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  return 0;
}

// Loading data.db
TEST database_loading() {
  log_debug("sqlite3_libversion: %s\n", sqlite3_libversion());

  platform_path databasePath = get_executable_dir_path();
  platform_path_append(&databasePath, "/assets/data.db");

  sqlite3 *db; // database connection handle

  int rc = sqlite3_open(platform_path_c_str(&databasePath), &db);
  if (rc != SQLITE_OK) {
    log_error("database error: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    FAILm("failed to data.db file");
  }

  char *sql = "DROP TABLE IF EXISTS Resolutions;"
              "CREATE TABLE Resolutions(width INT, height INT);"
              "INSERT INTO Resolutions VALUES(640, 460);"
              "INSERT INTO Resolutions VALUES(800, 600);"
              "INSERT INTO Resolutions VALUES(1024, 768);"
              "INSERT INTO Resolutions VALUES(1280, 720);"
              "INSERT INTO Resolutions VALUES(1280, 800);"
              "INSERT INTO Resolutions VALUES(1280, 1024);";
  rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    log_error("database error: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    FAILm("failed to execute sql insert query");
  }

  // first method
  sql = "SELECT * FROM Resolutions";
  rc = sqlite3_exec(db, sql, database_resolutions_callback, NULL, NULL);
  if (rc != SQLITE_OK) {
    log_error("database error: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    FAILm("failed to execute sql select query (1)");
  }
  // second method
  sqlite3_stmt *stmt;
  sql = "SELECT width, height FROM Resolutions";
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    log_error("database error: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    FAILm("failed to prepare sql select query (2)");
  }
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int width = sqlite3_column_int(stmt, 0);
    int height = sqlite3_column_int(stmt, 1);
    log_debug("%dx%d\n", width, height);
  }
  if (rc != SQLITE_DONE) {
    log_error("database error: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    FAILm("failed to execute sql select query (2)");
  }
  sqlite3_finalize(stmt);

  log_debug("closing\n");
  sqlite3_close(db);

  data_config config = data_config_init();
  log_debug("config.windowWidth = %d\n", config.windowWidth);
  log_debug("config.windowHeight = %d\n", config.windowHeight);
  log_debug("config.windowTitle = %s\n", str_c_str(&config.windowTitle));
  data_config_free(&config);
  PASS();
}

SUITE(database_suite) { RUN_TEST(database_loading); }

// Loading sponza.gltf.
TEST gltf_loading() {
  // TODO: Files in database.
  // TODO: Loading extra files (images).
  // platform_path gltfPath = get_asset_file_path("sponza", "Sponza.gltf");
  // platform_path gltfPath = get_asset_file_path("triangle", "Triangle.gltf");
  platform_path gltfPath = get_asset_file_path("triangles", "triangles.gltf");
  vulkan_scene *scene = vulkan_scene_create_with_gltf_file(gltfPath);
  vulkan_scene_debug_print(scene);
  platform_path_deinit(&gltfPath);
  vulkan_scene_destroy(scene);
  PASS();
}

SUITE(gltf_suite) { RUN_TEST(gltf_loading); }

// Memory allocation.
TEST platform_alloc() {
  /*vulkan_limits *limits = alloc_struct(vulkan_limits);
  ASSERT_EQ(vulkan_limits_alloc_stats.allocNum, 1);
  ASSERT_EQ(count_struct_array(limits), 1);
  dealloc_struct(limits);
  ASSERT_EQ(vulkan_limits_alloc_stats.allocNum, 0);
  ASSERT_EQ(limits, NULL);
  ASSERT_EQ(count_struct_array(limits), 0);
  vulkan_limits *limits2 = alloc_struct(vulkan_limits);
  limits = alloc_struct_array(vulkan_limits, 3);
  ASSERT_EQ(vulkan_limits_alloc_stats.allocNum, 4);
  ASSERT_EQ(count_struct_array(limits), 3);
  ASSERT_EQ(count_struct_array(limits2), 1);
  dealloc_struct(limits);
  ASSERT_EQ(vulkan_limits_alloc_stats.allocNum, 1);
  ASSERT_EQ(limits, NULL);
  ASSERT_EQ(count_struct_array(limits), 0);
  dealloc_struct(limits2);
  ASSERT_EQ(vulkan_limits_alloc_stats.allocNum, 0);
  ASSERT_EQ(limits2, NULL);*/
  PASS();
}

SUITE(platform_alloc_suite) { RUN_TEST(platform_alloc); }

TEST shaderc_compiling() {
  platform_path vertInputPath = get_asset_file_path("shaders", "shader.vert");
  platform_path fragInputPath = get_asset_file_path("shaders", "shader.frag");
  data_config config = data_config_init();
  vulkan_device *vkd = alloc_struct(vulkan_device);
  init_struct(vkd, vulkan_device_create, &config);
  vulkan_shader *vertShader = alloc_struct(vulkan_shader);
  init_struct(vertShader, vulkan_shader_create_with_path, vkd, vertInputPath);
  vulkan_shader *fragShader = alloc_struct(vulkan_shader);
  init_struct(fragShader, vulkan_shader_create_with_path, vkd, fragInputPath);

  ASSERT_EQ(vertShader->type, shaderc_glsl_vertex_shader);
  ASSERT_EQ(fragShader->type, shaderc_glsl_fragment_shader);
  log_debug("maxVertexInputAttributes=%d", vkd->limits.maxVertexInputAttributes);
  log_debug("maxVertexOutputComponents/4=%d", vkd->limits.maxVertexOutputComponents / 4);
  size_t inputAttributeCount = count_struct_array(vertShader->info.inputAttributeDescriptions);
  size_t outputAttributeCount = count_struct_array(vertShader->info.outputAttributeDescriptions);
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
  data_config_free(&config);
  dealloc_struct(vertShader);
  dealloc_struct(fragShader);
  dealloc_struct(vkd);
  PASS();
}

SUITE(shaderc_suite) { RUN_TEST(shaderc_compiling); }

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();
  platform_init();
  log_info("start test suite");
  //RUN_SUITE(c_parser_suite);
  //RUN_SUITE(database_suite);
  RUN_SUITE(gltf_suite);
  //RUN_SUITE(platform_alloc_suite);
  //RUN_SUITE(shaderc_suite);
  log_info("finish test suite");
  platform_free();
  GREATEST_MAIN_END();
}

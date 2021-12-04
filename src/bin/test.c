#include "../peg/c_parser.h"
#include "../platform.h"
#include "greatest.h"
#include "vulkan/vulkan.h"

#define P
#define T int
#include "vec.h"
int vec_int_compare(int *a, int *b) { return *b < *a; }

typedef struct image {
  char *data;
  size_t dataSize;
} image;

void image_free(image *self) {
  free(self->data);
  self->data = NULL;
  self->dataSize = 0;
}

image image_copy(image *self) {
  image copy = {0};
  copy.data = malloc(self->dataSize);
  memcpy(copy.data, self->data, self->dataSize);
  copy.dataSize = self->dataSize;
  return copy;
}

#define T image
#include "vec.h"

image image_init(size_t dataCount) {
  image self = {.data = malloc(sizeof(*self.data) * dataCount),
                .dataSize = sizeof(*self.data) * dataCount};
  return self;
}

// Basic test template.
TEST basic_test_template() { // NOLINT

  vec_int a = vec_int_init();
  vec_int_push_back(&a, 9); // NOLINT
  vec_int_push_back(&a, 1); // NOLINT
  vec_int_push_back(&a, 8); // NOLINT
  vec_int_push_back(&a, 3); // NOLINT
  vec_int_push_back(&a, 4); // NOLINT
  vec_int_sort(&a, vec_int_compare);
  int last = 0;
  for (size_t i = 0; i < a.size; i++) {
    ASSERT_LT(last, a.value[i]);
    last = a.value[i];
  }
  vec_int_free(&a);

  vec_image b = vec_image_init();
  vec_image_push_back(&b, image_init(10));  // NOLINT
  vec_image_push_back(&b, image_init(10));  // NOLINT
  vec_image_push_back(&b, image_init(100)); // NOLINT
  vec_image b2 = vec_image_copy(&b);
  for (size_t i = 0; i < b.size; i++) {
    ASSERT_EQ(b.value[i].dataSize, b2.value[i].dataSize);
    ASSERT_MEM_EQ(b.value[i].data, b2.value[i].data,
                  sizeof(*b.value[i].data) * b.value[i].dataSize);
  }
  PASS();
}

SUITE(basic_test_suite) { RUN_TEST(basic_test_template); }

#include <stdlib.h>

// Parsing C preprocessor directives.
TEST c_parser_preprocessor_parsing() {
  size_t inputSize;
  platform_path inputPath = get_executable_dir_path();
  platform_path_append(&inputPath, "/tests/c_parser_test.txt");
  char *input = read_text_file(&inputPath, &inputSize);
  platform_path_free(&inputPath);
  if (input == NULL) {
    FAILm("failed to load file");
  }
  c_parser_state state = c_parser_execute(input);
  c_parser_debug_print(&state);
  c_parser_state_free(&state);
  PASS();
}

SUITE(c_parser_suite) { RUN_TEST(c_parser_preprocessor_parsing); }

#include "../data/config.h"
#include <sqlite3.h>

int database_resolutions_callback(void *callbackData, int argc, char **argv,
                                  char **azColName) {
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

  int rc = sqlite3_open(str_c_str(&databasePath.data), &db);
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

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();
  platform_init();
  log_info("start test suite");
  RUN_SUITE(basic_test_suite);
  RUN_SUITE(c_parser_suite);
  RUN_SUITE(database_suite);
  log_info("finish test suite");
  platform_free();
  GREATEST_MAIN_END();
}

#include "config.h"
#include "../codegen/config.c"

data_config data_config_init() {
  data_config config = {0};
  platform_path databasePath = get_asset_file_path("", "data.db");
  int rc = sqlite3_open(platform_path_c_str(&databasePath), &config.db);
  platform_path_deinit(&databasePath);
  if (rc != SQLITE_OK) {
    panic("database error (1): %s", sqlite3_errmsg(config.db));
  }
  config.windowWidth = data_config_get_int(&config, "windowWidth");
  config.windowHeight = data_config_get_int(&config, "windowHeight");
  config.windowTitle = data_config_get_str(&config, "windowTitle");
  return config;
}

void data_config_free(data_config *config) {
  config->windowWidth = 0;
  config->windowHeight = 0;
  str_free(&config->windowTitle);
  sqlite3_close(config->db);
}

int data_config_get_int(data_config *config, char *key) {
  sqlite3_stmt *stmt;
  char *sql = "SELECT key, value FROM config WHERE key = ?;";
  int rc = sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    panic("database error (2): %s", sqlite3_errmsg(config->db));
  }
  sqlite3_bind_text(stmt, 1, key, strlen(key), NULL);
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    panic("database error (3): %s", sqlite3_errmsg(config->db));
  }
  int value = sqlite3_column_int(stmt, 1);
  sqlite3_finalize(stmt);
  return value;
}

str data_config_get_str(data_config *config, char *key) {
  sqlite3_stmt *stmt;
  char *sql = "SELECT key, value FROM config WHERE key = ?;";
  int rc = sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    panic("database error (2): %s", sqlite3_errmsg(config->db));
  }
  sqlite3_bind_text(stmt, 1, key, strlen(key), NULL);
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    panic("database error (3): %s", sqlite3_errmsg(config->db));
  }
  const unsigned char *result = sqlite3_column_text(stmt, 1);
  str value = str_init((const char *)result);
  sqlite3_finalize(stmt);
  return value;
}

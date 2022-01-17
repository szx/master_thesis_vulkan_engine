#include "assets.h"

data_assets *data_assets_create() {
  data_assets *assets = core_alloc(sizeof(data_assets));
  assets->path = globals.assetsFilePath;
  assets->db = data_db_create(assets->path);
  assets->config = data_config_create();
  return assets;
}

void data_assets_destroy(data_assets *assets) {
  data_assets_save(assets);
  data_db_destroy(assets->db);
  data_config_destroy(assets->config);
  core_free(assets);
}

void data_assets_save(data_assets *assets) {
  // Save only config - assumes that mesh blobs are read-only.
  data_config_save(assets->config);
}

void data_assets_save_empty(data_assets *assets) {
  // recreate tables
#define create_table(table, value_names, ...)                                                      \
  data_db_create_key_value_table_for_multiple_values(assets->db, #table, value_names);
  DATA_ASSETS_TABLES(create_table, )
#undef create_table
  data_assets_save(assets);
}

void data_assets_insert_primitive_int(data_assets *assets, char *key, char *column, int value) {
  data_db_insert_int(assets->db, "primitive", key, column, value);
}

void data_assets_insert_primitive_blob(data_assets *assets, char *key, char *column, void *value,
                                       size_t size) {
  data_db_insert_blob(assets->db, "primitive", key, column, value, size);
}
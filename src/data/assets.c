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
#define create_blob_table(table, ...) data_db_create_key_value_table_for_blobs(assets->db, #table);
  DATA_ASSETS_BLOB_TABLES(create_blob_table, )
#undef create_blob_table
  data_assets_save(assets);
}

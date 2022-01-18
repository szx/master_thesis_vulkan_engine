#include "asset_db.h"

data_asset_db *data_asset_db_create() {
  data_asset_db *assetDb = core_alloc(sizeof(data_asset_db));
  assetDb->path = globals.assetDatabaseFilepath;
  assetDb->db = data_db_create(assetDb->path);
  return assetDb;
}

void data_asset_db_destroy(data_asset_db *assetDb) {
  data_asset_db_save(assetDb);
  data_db_destroy(assetDb->db);
  core_free(assetDb);
}

void data_asset_db_save(data_asset_db *assetDb) {
  // No-op - assumes that asset database are read-only.
}

void data_asset_db_save_empty(data_asset_db *assetDb) {
  // recreate tables
#define create_table(table, key_def, value_defs, ...)                                              \
  data_db_create_key_value_table_for_multiple_values(assetDb->db, #table, key_def, value_defs);
  DATA_ASSET_DB_TABLES(create_table, )
#undef create_table
  data_asset_db_save(assetDb);
}

void data_asset_db_insert_primitive_int(data_asset_db *assetDb, void *key, size_t keySize,
                                        char *column, int value) {
  data_db_insert_int(assetDb->db, "primitive", key, keySize, column, value, true);
}

void data_asset_db_insert_primitive_blob(data_asset_db *assetDb, void *key, size_t keySize,
                                         char *column, void *value, size_t size) {
  data_db_insert_blob(assetDb->db, "primitive", key, keySize, column, value, size, true);
}

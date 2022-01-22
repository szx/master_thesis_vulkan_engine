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
#define ARG_TO_STR(_i, _elem) #_elem,
#define create_table(_table, _key_def, ...)                                                        \
  do {                                                                                             \
    const char *_args[] = {MACRO_FOREACH(ARG_TO_STR, __VA_ARGS__)};                                \
    data_db_create_key_value_table_for_multiple_values(assetDb->db, #_table, _key_def, _args,      \
                                                       MACRO_NARGS(__VA_ARGS__) - 1);              \
  } while (0);
  DATA_ASSET_DB_TABLES(create_table, )
#undef create_table
#undef ARG_TO_STR
  data_asset_db_save(assetDb);
}

#define def_insert(_table, _column, _type, ...)                                                    \
  void data_asset_db_insert_##_table##_##_column##_##_type(data_asset_db *assetDb, void *key,      \
                                                           size_t keySize, data##_##_type value) { \
    data_db_insert_##_type(assetDb->db, #_table, key, keySize, #_column, value, true);             \
  }
DATA_ASSET_DB_COLUMNS(def_insert, )
#undef def_insert

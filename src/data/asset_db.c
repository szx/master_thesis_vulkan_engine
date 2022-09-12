#include "asset_db.h"

asset_db *asset_db_create() {
  asset_db *assetDb = core_alloc(sizeof(asset_db));
  assetDb->path = globals.assetDatabaseFilepath;
  assetDb->db = sql_db_create(assetDb->path);
  return assetDb;
}

void asset_db_destroy(asset_db *assetDb) {
  asset_db_save(assetDb);
  sql_db_destroy(assetDb->db);
  core_free(assetDb);
}

void asset_db_save(asset_db *assetDb) {
  // No-op - assumes that asset database are read-only.
}

void asset_db_save_empty(asset_db *assetDb) {
  // recreate tables
#define ARG_TO_STR(_i, _elem) #_elem,
#define create_table(_table, _key_name, _key_type, ...)                                            \
  do {                                                                                             \
    const char *_args[] = {MACRO_FOREACH(ARG_TO_STR, __VA_ARGS__)};                                \
    sql_db_create_key_value_table_for_multiple_values(                                             \
        assetDb->db, #_table, #_key_name, #_key_type, _args, MACRO_NARGS(__VA_ARGS__) - 1);        \
  } while (0);
  DATA_ASSET_DB_TABLES(create_table, )
#undef create_table
#undef ARG_TO_STR
  asset_db_save(assetDb);
}

/* select */

#define def_select(_table, _column, _type, ...)                                                    \
  data##_##_type asset_db_select_##_table##_##_column##_##_type(asset_db *assetDb, data_key key) { \
    return sql_db_select_##_type(assetDb->db, #_table, key, #_column);                             \
  }
DATA_ASSET_DB_COLUMNS(def_select, )
#undef def_select

/* insert */

#define def_insert(_table, _column, _type, ...)                                                    \
  void asset_db_insert_##_table##_##_column##_##_type(asset_db *assetDb, data_key key,             \
                                                      data##_##_type value) {                      \
    sql_db_insert_##_type(assetDb->db, #_table, key, #_column, value, true);                       \
  }
DATA_ASSET_DB_COLUMNS(def_insert, )
#undef def_insert

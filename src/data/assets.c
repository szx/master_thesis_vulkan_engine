#include "assets.h"

data_assets *data_assets_create(bool shouldLoad) {
  data_assets *assets = core_alloc(sizeof(data_assets));
  UT_string *path = get_asset_file_path("", "data.db");
  assets->db = data_db_create(path);
  utstring_free(path);
  if (shouldLoad) {
    // Load values.
#define load_int(table, key, ...) assets->key = data_db_get_int(assets->db, #key);
#define load_string(table, key, ...) assets->key = data_db_get_str(assets->db, #key);
    DATA_ASSETS_KEYS(load_int, load_string, )
#undef load_string
#undef load_int
  } else {
    // Insert default values.
    int defaultInt = 0;
    UT_string *defaultStr;
    utstring_new(defaultStr);
#define save_int(table, key, ...) assets->key = defaultInt;
#define save_string(table, key, ...)                                                               \
  utstring_new(assets->key);                                                                       \
  utstring_concat(assets->key, defaultStr);
    DATA_ASSETS_KEYS(save_int, save_string, )
#undef save_string
#undef save_int
    utstring_free(defaultStr);
    data_assets_save_default(assets);
  }
  return assets;
}

void data_assets_destroy(data_assets *assets) {
  data_assets_save(assets);
  data_db_destroy(assets->db);
#define free_int(table, key, ...)
#define free_string(table, key, ...) utstring_free(assets->windowTitle);
  DATA_ASSETS_KEYS(free_int, free_string, )
#undef free_string
#undef free_int
  core_free(assets);
}

void data_assets_save(data_assets *assets) {
  // Save text values.
  // We assume that blobs are read-only.
#define save_int(table, key, ...) data_db_insert_int(assets->db, #table, #key, assets->key);
#define save_string(table, key, ...) data_db_insert_str(assets->db, #table, #key, assets->key);
  DATA_ASSETS_KEYS(save_int, save_string, )
#undef save_string
#undef save_int
}

void data_assets_save_default(data_assets *assets) {
  // create tables
#define create_text_table(table, ...) data_db_create_key_value_table_for_text(assets->db, #table);
#define create_blob_table(table, ...) data_db_create_key_value_table_for_blobs(assets->db, #table);
  DATA_ASSETS_TABLES(create_text_table, create_blob_table, )
#undef create_text_table
#undef create_blob_table
  data_assets_save(assets);
}

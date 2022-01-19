/* SQLite database as a asset database. */
#pragma once

#include "config.h"
#include "db.h"

#include "../codegen/macros.h"

typedef struct data_asset_db {
  data_db *db;     /// SQLite database file.
  UT_string *path; /// SQLite database filepath.
  // TODO: Fetching mesh blobs to CPU.
} data_asset_db;

data_asset_db *data_asset_db_create();
void data_asset_db_destroy(data_asset_db *assetDb);
/// Saves assets database.
void data_asset_db_save(data_asset_db *assetDb);
/// Saves empty assets database.
/// Used by asset pipeline.
void data_asset_db_save_empty(data_asset_db *assetDb);

#define decl_insert(_table, _column, _type, ...)                                                   \
  void data_asset_db_insert_##_table##_##_column##_##_type(data_asset_db *assetDb, void *key,      \
                                                           size_t keySize, data##_##_type value);
DATA_ASSET_DB_COLUMNS(decl_insert, )
#undef decl_insert

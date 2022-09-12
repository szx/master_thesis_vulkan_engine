/* SQLite database as a asset database. */
#pragma once

#include "config.h"
#include "db.h"

typedef struct asset_db {
  sql_db *db;      /// SQLite database file.
  UT_string *path; /// SQLite database filepath.
} asset_db;

asset_db *asset_db_create();
void asset_db_destroy(asset_db *assetDb);
/// Saves assets database.
void asset_db_save(asset_db *assetDb);
/// Saves empty assets database.
/// Used by asset pipeline.
void asset_db_save_empty(asset_db *assetDb);

/* select */
#define decl_select(_table, _column, _type, ...)                                                   \
  data##_##_type asset_db_select_##_table##_##_column##_##_type(asset_db *assetDb, data_key key);
DATA_ASSET_DB_COLUMNS(decl_select, )
#undef decl_select

/* insert */
#define decl_insert(_table, _column, _type, ...)                                                   \
  void asset_db_insert_##_table##_##_column##_##_type(asset_db *assetDb, data_key key,             \
                                                      data##_##_type value);
DATA_ASSET_DB_COLUMNS(decl_insert, )
#undef decl_insert

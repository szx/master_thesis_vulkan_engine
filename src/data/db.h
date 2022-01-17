/* SQLite database helpers. */
#pragma once

#include "../core/core.h"

/// SQLite database file.
typedef struct data_db {
  UT_string *path;
  sqlite3 *db;
} data_db;

data_db *data_db_create(UT_string *path);
void data_db_destroy(data_db *db);

/// Returns int value of a text key.
int data_db_get_int(data_db *db, char *key);

/// Returns new string value of a text key.
UT_string *data_db_get_str(data_db *db, char *key);

/// Returns inserted int value for a text key.
int data_db_insert_int(data_db *db, char *table, char *key, char *column, int value);

/// Returns inserted string value for a text key.
UT_string *data_db_insert_str(data_db *db, char *table, char *key, UT_string *value);

/// Inserts blob for a text key.
void data_db_insert_blob(data_db *db, char *table, char *key, char *column, void *value,
                         size_t size);

// TODO: Insert blob.

/// Creates table with key/value (text/text) schema.
/// NOTE: SQLite has "dynamic type system", which means that we can store
///       integers in text fields.
void data_db_create_key_value_table_for_text(data_db *db, char *table);

/// Creates table with key/value (text/blobs) schema.
void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *valueNames);

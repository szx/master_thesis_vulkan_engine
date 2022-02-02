/* SQLite database helpers. */
#pragma once

#include "../core/core.h"

#include "../codegen/macros.h"

/// SQLite database file.
typedef struct data_db {
  UT_string *path;
  sqlite3 *db;
} data_db;

data_db *data_db_create(UT_string *path);
void data_db_destroy(data_db *db);

/* data types */

#define decl_data_type(_type, _value, ...)                                                         \
  typedef struct data##_##_type {                                                                  \
    _value value;                                                                                  \
  } data##_##_type;                                                                                \
  void data##_##_type##_init(data_##_type *data, _value value);                                    \
  void data##_##_type##_deinit(data_##_type *data);                                                \
  typedef struct data##_##_type##_array {                                                          \
    UT_array *values;                                                                              \
  } data##_##_type##_array;                                                                        \
  void data_##_type##_array_init(data_##_type##_array *array);                                     \
  void data_##_type##_array_deinit(data_##_type##_array *array);                                   \
  void data_##_type##_array_push_back(data_##_type##_array *array, _value value);
DATA_DB_TYPES(decl_data_type, )
#undef decl_data_type

/* serialize */

#define decl_data_serialize(_type, _value, ...)                                                    \
  size_t data_size_##_type(_value value);                                                          \
  void data_serialize_##_type(uint8_t *memory, _value value);                                      \
  void data_deserialize_##_type(const uint8_t *memory, size_t size, _value *value);
DATA_DB_TYPES(decl_data_serialize, )
#undef decl_data_serialize

/* select */

#define decl_data_select(_type, _value, ...)                                                       \
  data_##_type data_db_select_##_type(data_db *db, char *table, data_key key, char *column);       \
  data_##_type##_array data_db_select_##_type##_array(data_db *db, char *table, data_key key,      \
                                                      char *column);
DATA_DB_TYPES(decl_data_select, )
#undef decl_data_select

/* insert */

#define decl_data_insert(_type, _value, ...)                                                       \
  void data_db_insert_##_type(data_db *db, char *table, data_key key, char *column,                \
                              data_##_type data, bool updateIfExists);                             \
  void data_db_insert_##_type##_array(data_db *db, char *table, data_key key, char *column,        \
                                      data_##_type##_array array, bool updateIfExists);
DATA_DB_TYPES(decl_data_insert, )
#undef decl_data_insert

/* create */

/// Creates table with key/value (text/blobs) schema.
/// NOTE: SQLite has "dynamic type system", which means that we can store
///       integers in text fields.
void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyName, const char *keyType,
                                                        const char *columnDefs[],
                                                        size_t columnDefsCount);

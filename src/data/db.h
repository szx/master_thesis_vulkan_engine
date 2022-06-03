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
  typedef struct data_##_type {                                                                    \
    _value value;                                                                                  \
  } data_##_type;                                                                                  \
  void data_##_type##_init(data_##_type *data, _value value);                                      \
  data_##_type data_##_type##_temp(_value value); /* NOTE: can leak memory */                      \
  size_t data_##_type##_size(_value value);                                                        \
  void data_##_type##_serialize(uint8_t *memory, _value value);                                    \
  void data_##_type##_deserialize(const uint8_t *memory, size_t size, _value *value);              \
                                                                                                   \
  typedef struct data##_##_type##_array {                                                          \
    UT_array *values;                                                                              \
  } data##_##_type##_array;                                                                        \
  void data_##_type##_array_init(data_##_type##_array *array, size_t count);                       \
  void data_##_type##_array_deinit(data_##_type##_array *array);                                   \
  data_##_type##_array data_##_type##_array_temp(UT_array *values);
DATA_DB_TYPES(decl_data_type, )
#undef decl_data_type

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

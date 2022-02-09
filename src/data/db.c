#include "db.h"

data_db *data_db_create(UT_string *path) {
  data_db *db = core_alloc(sizeof(data_db));
  utstring_new(db->path);
  utstring_concat(db->path, path);
  log_debug("opening SQLite database '%s'", utstring_body(db->path));
  int rc = sqlite3_open(utstring_body(db->path), &db->db);
  if (rc != SQLITE_OK) {
    panic("database error (1): %s", sqlite3_errmsg(db->db));
  }
  return db;
}

void data_db_destroy(data_db *db) {
  utstring_free(db->path);
  sqlite3_close(db->db);
  core_free(db);
}

/* data types */

void data_byte_init(data_byte *data, uint8_t value) { data->value = value; }
void data_byte_deinit(data_byte *data) {}
size_t data_byte_size(uint8_t value) { return sizeof(uint8_t); }
void data_byte_serialize(uint8_t *memory, uint8_t value) { *(uint8_t *)memory = value; }
void data_byte_deserialize(const uint8_t *memory, size_t size, uint8_t *value) {
  verify(size == sizeof(uint8_t));
  *value = *(uint8_t *)memory;
}

void data_int_init(data_int *data, uint32_t value) { data->value = value; }
void data_int_deinit(data_int *data) {}
size_t data_int_size(uint32_t value) { return sizeof(uint32_t); }
void data_int_serialize(uint8_t *memory, uint32_t value) { *(uint32_t *)memory = value; }
void data_int_deserialize(const uint8_t *memory, size_t size, uint32_t *value) {
  verify(size == sizeof(uint32_t));
  *value = *(uint32_t *)memory;
}

void data_float_init(data_float *data, float value) { data->value = value; }
void data_float_deinit(data_float *data) {}
size_t data_float_size(float value) { return sizeof(float); }
void data_float_serialize(uint8_t *memory, float value) { *(float *)memory = value; }
void data_float_deserialize(const uint8_t *memory, size_t size, float *value) {
  verify(size == sizeof(float));
  *value = *(float *)memory;
}

void data_vec2_init(data_vec2 *data, vec2 value) { glm_vec2_copy(value, data->value); }
void data_vec2_deinit(data_vec2 *data) {}
size_t data_vec2_size(vec2 value) { return sizeof(vec2); }
void data_vec2_serialize(uint8_t *memory, vec2 value) {
  float array[2] = {value[0], value[1]};
  memcpy(memory, array, sizeof(array));
}
void data_vec2_deserialize(const uint8_t *memory, size_t size, vec2 *value) {
  verify(size == sizeof(vec2));
  const float *ptr = (const float *)memory;
  (*value)[0] = ptr[0];
  (*value)[1] = ptr[1];
}

void data_vec3_init(data_vec3 *data, vec3 value) { glm_vec3_copy(value, data->value); }
void data_vec3_deinit(data_vec3 *data) {}
size_t data_vec3_size(vec3 value) { return sizeof(vec3); }
void data_vec3_serialize(uint8_t *memory, vec3 value) {
  float array[3] = {value[0], value[1], value[2]};
  memcpy(memory, array, sizeof(array));
}
void data_vec3_deserialize(const uint8_t *memory, size_t size, vec3 *value) {
  verify(size == sizeof(vec3));
  const float *ptr = (const float *)memory;
  (*value)[0] = ptr[0];
  (*value)[1] = ptr[1];
  (*value)[2] = ptr[2];
}

void data_vec4_init(data_vec4 *data, vec4 value) { glm_vec4_copy(value, data->value); }
void data_vec4_deinit(data_vec4 *data) {}
size_t data_vec4_size(vec4 value) { return sizeof(vec4); }
void data_vec4_serialize(uint8_t *memory, vec4 value) {
  float array[4] = {value[0], value[1], value[2], value[3]};
  memcpy(memory, array, sizeof(array));
}
void data_vec4_deserialize(const uint8_t *memory, size_t size, vec4 *value) {
  verify(size == sizeof(vec4));
  const float *ptr = (const float *)memory;
  (*value)[0] = ptr[0];
  (*value)[1] = ptr[1];
  (*value)[2] = ptr[2];
  (*value)[3] = ptr[3];
}

void data_mat4_init(data_mat4 *data, mat4 value) { glm_mat4_copy(value, data->value); }
void data_mat4_deinit(data_mat4 *data) {}
size_t data_mat4_size(mat4 value) { return sizeof(float[16]); }
void data_mat4_serialize(uint8_t *memory, mat4 value) {
  float mat[16] = {
      value[0][0], value[0][1], value[0][2], value[0][3], value[1][0], value[1][1],
      value[1][2], value[1][3], value[2][0], value[2][1], value[2][2], value[2][3],
      value[3][0], value[3][1], value[3][2], value[3][3],
  };
  memcpy(memory, mat, sizeof(mat));
}
void data_mat4_deserialize(const uint8_t *memory, size_t size, mat4 *value) {
  verify(size == sizeof(mat4));
  const float *ptr = (const float *)memory;
  (*value)[0][0] = ptr[0];
  (*value)[0][1] = ptr[1];
  (*value)[0][2] = ptr[2];
  (*value)[0][3] = ptr[3];
  (*value)[1][0] = ptr[4];
  (*value)[1][1] = ptr[5];
  (*value)[1][2] = ptr[6];
  (*value)[1][3] = ptr[7];
  (*value)[2][0] = ptr[8];
  (*value)[2][1] = ptr[9];
  (*value)[2][2] = ptr[10];
  (*value)[2][3] = ptr[11];
  (*value)[3][0] = ptr[12];
  (*value)[3][1] = ptr[13];
  (*value)[3][2] = ptr[14];
  (*value)[3][3] = ptr[15];
}

void data_text_init(data_text *data, UT_string *value) {
  utstring_new(data->value);
  utstring_concat(data->value, value);
  utstring_free(value);
}
void data_text_deinit(data_text *data) { utstring_free(data->value); }
size_t data_text_size(UT_string *value) { return utstring_len(value); }
void data_text_serialize(uint8_t *memory, UT_string *value) {
  for (size_t i = 0; i < utstring_len(value); i++) {
    memory[i] = utstring_body(value)[i];
  }
}
void data_text_deserialize(const uint8_t *memory, size_t size, UT_string **value) {
  utstring_renew(*value);
  utstring_bincpy(*value, memory, size);
}

void data_key_init(data_key *data, uint64_t value) { data->value = value; }
void data_key_deinit(data_key *data) {}
size_t data_key_size(hash_t value) { return sizeof(value); }
void data_key_serialize(uint8_t *memory, hash_t value) { *(hash_t *)memory = value; }
void data_key_deserialize(const uint8_t *memory, size_t size, hash_t *value) {
  if (size == sizeof(hash_t)) {
    *value = *(hash_t *)memory;
  } else {
    verify(size == 0);
    *value = 0;
  }
}

#define def_data_type_temp(_type, _value, ...)                                                     \
  data_##_type data##_##_type##_temp(_value value) {                                               \
    data##_##_type temp = {0};                                                                     \
    data##_##_type##_init(&temp, value);                                                           \
    return temp;                                                                                   \
  }                                                                                                \
  data_##_type##_array data_##_type##_array_temp(UT_array *values) {                               \
    data##_##_type##_array temp = {values};                                                        \
    return temp;                                                                                   \
  }
DATA_DB_TYPES(def_data_type_temp, )
#undef def_data_type_temp

#define def_data_type_array(_type, _value, ...)                                                    \
  void data##_##_type##_array_init(data##_##_type##_array *array) {                                \
    utarray_alloc(array->values, sizeof(data##_##_type));                                          \
  }                                                                                                \
  void data##_##_type##_array_deinit(data##_##_type##_array *array) {                              \
    data##_##_type *data = NULL;                                                                   \
    while ((data = (utarray_next(array->values, data)))) {                                         \
      data##_##_type##_deinit(data);                                                               \
    }                                                                                              \
    utarray_free(array->values);                                                                   \
  }                                                                                                \
  void data##_##_type##_array_push_back(data##_##_type##_array *array, _value value) {             \
    data##_##_type data;                                                                           \
    data##_##_type##_init(&data, value);                                                           \
    utarray_push_back(array->values, &data);                                                       \
  }
DATA_DB_TYPES(def_data_type_array, )
#undef def_data_type_array

/* select */

#define SQLITE_PREPARE(query, ...)                                                                 \
  do {                                                                                             \
    sqlite3_stmt *_stmt;                                                                           \
    UT_string *_sql;                                                                               \
    utstring_new(_sql);                                                                            \
    utstring_printf(_sql, query, __VA_ARGS__);                                                     \
    log_debug("SQLITE_PREPARE(%s)", utstring_body(_sql));                                          \
    int _rc = sqlite3_prepare_v2(db->db, utstring_body(_sql), -1, &_stmt, NULL);                   \
    if (_rc != SQLITE_OK) {                                                                        \
      panic("database error (prepare): %s", sqlite3_errmsg(db->db));                               \
    }                                                                                              \
    utstring_free(_sql);

#define SQLITE_STEP(expected)                                                                      \
  _rc = sqlite3_step(_stmt);                                                                       \
  if (_rc != expected) {                                                                           \
    panic("database error (step): %s", sqlite3_errmsg(db->db));                                    \
  }

#define SQLITE_FINALIZE()                                                                          \
  sqlite3_finalize(_stmt);                                                                         \
  }                                                                                                \
  while (0)

#define SQLITE_EXEC(query, ...)                                                                    \
  {                                                                                                \
    utstring_renew(sql);                                                                           \
    utstring_printf(sql, query, __VA_ARGS__);                                                      \
    log_debug("SQLITE_EXEC(%s)", utstring_body(sql));                                              \
    int rc = sqlite3_exec(db->db, utstring_body(sql), NULL, NULL, NULL);                           \
    if (rc != SQLITE_OK) {                                                                         \
      panic("database error (exec): %s", sqlite3_errmsg(db->db));                                  \
    }                                                                                              \
  }

#define def_data_select(_type, _value, ...)                                                        \
  data##_##_type data_db_select_##_type(data_db *db, char *table, data_key key, char *column) {    \
    data##_##_type data;                                                                           \
    SQLITE_PREPARE("SELECT key, %s FROM %s WHERE key = ?;", column, table);                        \
    sqlite3_bind_blob(_stmt, 1, &key.value, sizeof(key.value), NULL);                              \
    SQLITE_STEP(SQLITE_ROW);                                                                       \
    const void *memory = sqlite3_column_blob(_stmt, 1);                                            \
    size_t size = sqlite3_column_bytes(_stmt, 1);                                                  \
    _value value = {0};                                                                            \
    data_##_type##_deserialize(memory, size, &value);                                              \
    data_##_type##_init(&data, value);                                                             \
    SQLITE_FINALIZE();                                                                             \
    return data;                                                                                   \
  }
DATA_DB_TYPES(def_data_select, )
#undef def_data_select

#define def_data_array_select(_type, _value, ...)                                                  \
  data##_##_type##_array data_db_select_##_type##_array(data_db *db, char *table, data_key key,    \
                                                        char *column) {                            \
    data##_##_type##_array data;                                                                   \
    data##_##_type##_array_init(&data);                                                            \
    SQLITE_PREPARE("SELECT key, %s FROM %s WHERE key = ?;", column, table);                        \
    sqlite3_bind_blob(_stmt, 1, &key.value, sizeof(key.value), NULL);                              \
    SQLITE_STEP(SQLITE_ROW);                                                                       \
    const uint8_t *memory = sqlite3_column_blob(_stmt, 1);                                         \
    size_t size = sqlite3_column_bytes(_stmt, 1);                                                  \
    size_t elemSize = utarray_eltsize(data.values);                                                \
    verify(size % elemSize == 0);                                                                  \
    size_t count = size / elemSize;                                                                \
    for (size_t i = 0; i < count; i++) {                                                           \
      _value value = {0};                                                                          \
      data_##_type##_deserialize(memory + elemSize * i, elemSize, &value);                         \
      data##_##_type##_array_push_back(&data, value);                                              \
    }                                                                                              \
    SQLITE_FINALIZE();                                                                             \
    return data;                                                                                   \
  }
DATA_DB_TYPES(def_data_array_select, )
#undef def_data_array_select

/* insert */

#define def_data_insert(_type, _value, ...)                                                        \
  void data_db_insert_##_type(data_db *db, char *table, data_key key, char *column,                \
                              data_##_type data, bool updateIfExists) {                            \
    const char *query;                                                                             \
    if (updateIfExists) {                                                                          \
      query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO UPDATE SET %s = ?;";    \
    } else {                                                                                       \
      query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO NOTHING;";              \
    }                                                                                              \
    SQLITE_PREPARE(query, table, column, column);                                                  \
    sqlite3_bind_blob(_stmt, 1, &key.value, sizeof(key.value), NULL);                              \
    size_t size = data_##_type##_size(data.value);                                                 \
    uint8_t *memory = core_alloc(size);                                                            \
    data_##_type##_serialize(memory, data.value);                                                  \
    sqlite3_bind_blob(_stmt, 2, memory, size, NULL);                                               \
    if (updateIfExists) {                                                                          \
      sqlite3_bind_blob(_stmt, 3, memory, size, NULL);                                             \
    }                                                                                              \
    SQLITE_STEP(SQLITE_DONE);                                                                      \
    core_free(memory);                                                                             \
    SQLITE_FINALIZE();                                                                             \
  }
DATA_DB_TYPES(def_data_insert, )
#undef def_data_insert

#define def_data_array_insert(_type, _value, ...)                                                  \
  void data_db_insert_##_type##_array(data_db *db, char *table, data_key key, char *column,        \
                                      data_##_type##_array array, bool updateIfExists) {           \
    verify(utarray_eltsize(array.values) == sizeof(data_##_type)); /* FIXME: Forbid TEXT_ARRAY. */ \
    const char *query;                                                                             \
    if (updateIfExists) {                                                                          \
      query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO UPDATE SET %s = ?;";    \
    } else {                                                                                       \
      query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO NOTHING;";              \
    }                                                                                              \
    SQLITE_PREPARE(query, table, column, column);                                                  \
    sqlite3_bind_blob(_stmt, 1, &key.value, sizeof(key.value), NULL);                              \
    size_t size = utarray_size(array.values);                                                      \
    uint8_t *memory = core_alloc(size);                                                            \
    for (size_t i = 0; i < utarray_len(array.values); i++) {                                       \
      data_##_type *data = utarray_eltptr(array.values, i);                                        \
      data_##_type##_serialize(memory + sizeof(data_##_type) * i, data->value);                    \
    }                                                                                              \
    sqlite3_bind_blob(_stmt, 2, memory, size, NULL);                                               \
    if (updateIfExists) {                                                                          \
      sqlite3_bind_blob(_stmt, 3, memory, size, NULL);                                             \
    }                                                                                              \
    SQLITE_STEP(SQLITE_DONE);                                                                      \
    core_free(memory);                                                                             \
    SQLITE_FINALIZE();                                                                             \
  }
DATA_DB_TYPES(def_data_array_insert, )
#undef def_data_array_insert

/* create */

void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyName, const char *keyType,
                                                        const char *columnDefs[],
                                                        size_t columnDefsCount) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);

  if (strcmp(keyType, "hash") == 0) {
    // Hash is just a blob with special *_get() and *_insert().
    keyType = "blob";
  }
  UT_string *columns;
  utstring_new(columns);
  for (size_t i = 0; i < columnDefsCount; i += 2) {
    const char *name = columnDefs[i];
    const char *type = columnDefs[i + 1];
    if (strcmp(type, "hash") == 0 || strcmp(type, "hash_array") == 0 || strcmp(type, "mat4") == 0) {
      type = "blob";
    }
    utstring_printf(columns, "%s %s%s", name, type, (i + 2 < columnDefsCount ? "," : ""));
  }

  SQLITE_EXEC("CREATE TABLE %s(%s %s PRIMARY KEY, %s);", table, keyName, keyType,
              utstring_body(columns));
  utstring_free(columns);
}

/* Struct serialization. */
#pragma once

#include "junk.h"

#define SERIALIZE_DECL(_struct) data_blob _struct##_serialize(_struct *instance);

#define DESERIALIZE_DECL(_struct) void _struct##_deserialize(_struct *instance, data_blob blob);

#define SERIALIZE_DEF(_struct, ...)                                                                \
  data_blob _struct##_serialize(_struct *instance) {                                               \
    data_blob blob;                                                                                \
    blob.size = SERIALIZE_FIELD_SIZE(__VA_ARGS__);                                                 \
    blob.memory = core_alloc(blob.size);                                                           \
    uint8_t *ptr = blob.memory;                                                                    \
    size_t totalSize = 0;                                                                          \
    MACRO_FOREACH(SERIALIZE_FIELD, __VA_ARGS__)                                                    \
    return blob;                                                                                   \
  }

#define DESERIALIZE_DEF(_struct, ...)                                                              \
  void _struct##_deserialize(_struct *instance, data_blob blob) {                                  \
    uint8_t *ptr = blob.memory;                                                                    \
    size_t totalSize = 0;                                                                          \
    MACRO_FOREACH(DESERIALIZE_FIELD, __VA_ARGS__)                                                  \
  }

void serialize_float(uint8_t **ptr, size_t *size, float value);
float deserialize_float(uint8_t **ptr, size_t *size);

#define SERIALIZE_FIELD_SIZE(...) 0 MACRO_FOREACH(SERIALIZE_FIELD_SIZE_, __VA_ARGS__)
#define SERIALIZE_FIELD_SIZE_(_i, _elem) +sizeof(instance->_elem)

#define SERIALIZE_FIELD(_i, _elem) SERIALIZE_FIELD_(instance->_elem)
#define SERIALIZE_FIELD_(_elem)                                                                    \
  _Generic((_elem), float : serialize_float)(&ptr, &totalSize, _elem);                             \
  assert(totalSize <= blob.size);

#define DESERIALIZE_FIELD(_i, _elem) DESERIALIZE_FIELD_(instance->_elem)
#define DESERIALIZE_FIELD_(_elem)                                                                  \
  _elem = _Generic((_elem), float : deserialize_float)(&ptr, &totalSize);                          \
  assert(totalSize <= blob.size);

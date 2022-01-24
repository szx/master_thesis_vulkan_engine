#include "serialization.h"

void serialize_float(uint8_t *ptr, float value) { *(float *)ptr = value; }

void deserialize_float(uint8_t *ptr, float *value) { *value = *(float *)ptr; }

void serialize_vec3(uint8_t *ptr, vec3 value) {
  serialize_float(ptr, value[0]), ptr += sizeof(float);
  serialize_float(ptr, value[1]), ptr += sizeof(float);
  serialize_float(ptr, value[2]);
}

void deserialize_vec3(uint8_t *ptr, vec3 *value) {
  deserialize_float(ptr, &(*value)[0]), ptr += sizeof(float);
  deserialize_float(ptr, &(*value)[1]), ptr += sizeof(float);
  deserialize_float(ptr, &(*value)[2]);
}

void serialize_vec4(uint8_t *ptr, vec4 value) {
  serialize_float(ptr, value[0]), ptr += sizeof(float);
  serialize_float(ptr, value[1]), ptr += sizeof(float);
  serialize_float(ptr, value[2]), ptr += sizeof(float);
  serialize_float(ptr, value[3]);
}

void deserialize_vec4(uint8_t *ptr, vec4 *value) {
  deserialize_float(ptr, &(*value)[0]), ptr += sizeof(float);
  deserialize_float(ptr, &(*value)[1]), ptr += sizeof(float);
  deserialize_float(ptr, &(*value)[2]), ptr += sizeof(float);
  deserialize_float(ptr, &(*value)[3]);
}

void serialize_mat4(uint8_t *ptr, mat4 value) {
  serialize_vec4(ptr, value[0]), ptr += sizeof(vec4);
  serialize_vec4(ptr, value[1]), ptr += sizeof(vec4);
  serialize_vec4(ptr, value[2]), ptr += sizeof(vec4);
  serialize_vec4(ptr, value[3]);
}

void deserialize_mat4(uint8_t *ptr, mat4 *value) {
  deserialize_vec4(ptr, &(*value)[0]), ptr += sizeof(vec4);
  deserialize_vec4(ptr, &(*value)[1]), ptr += sizeof(vec4);
  deserialize_vec4(ptr, &(*value)[2]), ptr += sizeof(vec4);
  deserialize_vec4(ptr, &(*value)[3]);
}

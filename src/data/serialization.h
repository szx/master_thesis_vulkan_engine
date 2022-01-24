/* Struct serialization. */
#pragma once

#include "../core/junk.h"

#define SERIALIZE_FIELD_SIZE(...) 0 MACRO_FOREACH(SERIALIZE_FIELD_SIZE_, __VA_ARGS__)
#define SERIALIZE_FIELD_SIZE_(_i, _elem) +sizeof(_elem)

/* serialization helper functions */

void serialize_float(uint8_t *ptr, float value);
void deserialize_float(uint8_t *ptr, float *value);

void serialize_vec3(uint8_t *ptr, vec3 value);
void deserialize_vec3(uint8_t *ptr, vec3 *value);

void serialize_vec4(uint8_t *ptr, vec4 value);
void deserialize_vec4(uint8_t *ptr, vec4 *value);

void serialize_mat4(uint8_t *ptr, mat4 value);
void deserialize_mat4(uint8_t *ptr, mat4 *value);

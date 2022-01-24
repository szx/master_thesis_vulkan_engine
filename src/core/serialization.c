#include "serialization.h"

void serialize_float(uint8_t **ptr, size_t *size, float value) {
  *(float *)*ptr = value;
  *ptr += sizeof(value);
  *size += sizeof(value);
}

float deserialize_float(uint8_t **ptr, size_t *size) {
  float value = *(float *)*ptr;
  *ptr += sizeof(value);
  *size += sizeof(value);
  return value;
}

#include "alloc.h"

void *core_memdup(const void *src, size_t size) {
  void *data = malloc(size);
  memcpy(data, src, size);
  return data;
}


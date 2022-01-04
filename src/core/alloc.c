#include "alloc.h"

void *core_alloc(size_t size) {
  void *memory = malloc(size);
  // HIRO verify
  return memory;
}

void core_free(void *ptr) { free(ptr); }

void *core_memdup(const void *src, size_t size) {
  void *data = core_alloc(size);
  memcpy(data, src, size);
  return data;
}

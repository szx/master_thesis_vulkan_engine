#include "alloc.h"

void *core_alloc(size_t size) {
  if (size == 0) {
    return NULL;
  }
  void *memory = malloc(size);
  if (memory == NULL) {
    fprintf(stderr, "%s:%d: Out of memory (OOM)", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }
  return memory;
}

void core_free(void *ptr) { free(ptr); }

void *core_memcpy(void *dest, const void *src, size_t size) {
  if (dest == NULL || src == NULL || size == 0) {
    return dest;
  }
  return memcpy(dest, src, size);
}

void *core_memset(void *dest, int value, size_t size) {
  if (dest == NULL || size == 0) {
    return dest;
  }
  return memset(dest, value, size);
}

void *core_memdup(const void *src, size_t size) {
  void *data = core_alloc(size);
  memcpy(data, src, size);
  return data;
}

void *core_strdup(const char *dest) {
  if (dest == NULL) {
    return NULL;
  }
  return strdup(dest);
}

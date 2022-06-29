/* Basic allocation. */
#pragma once

#include "junk.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

void *core_alloc(size_t size);

void core_free(void *ptr);

void *core_memcpy(void *dest, const void *src, size_t size);

void *core_memset(void *dest, int value, size_t size);

void *core_strdup(const char *dest);

void *core_memdup(const void *src, size_t size);

int core_memcmp(const void *s1, const void *s2, size_t size);

#define utstring_alloc(_s, _str)                                                                   \
  do {                                                                                             \
    utstring_new(_s);                                                                              \
    utstring_printf(_s, "%s", _str);                                                               \
  } while (false)

#define utarray_alloc(_array, _size)                                                               \
  do {                                                                                             \
    const UT_icd icd = {_size, NULL, NULL, NULL};                                                  \
    utarray_new(_array, &icd);                                                                     \
  } while (false)

#define utarray_realloc(_array, _size)                                                             \
  do {                                                                                             \
    const UT_icd icd = {_size, NULL, NULL, NULL};                                                  \
    if (_array) {                                                                                  \
      utarray_free(_array);                                                                        \
    }                                                                                              \
    utarray_new(_array, &icd);                                                                     \
  } while (false)

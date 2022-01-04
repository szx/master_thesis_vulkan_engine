/* Basic allocation. */

#ifndef CORE_ALLOC_H
#define CORE_ALLOC_H

#include "junk.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

/// Allocates size bytes of memory.
void *core_alloc(size_t size);

/// Frees allocation.
void core_free(void *ptr);

/// Returns copy of memory.
void *core_memdup(const void *src, size_t size);

#define core_array(type)                                                                           \
  struct {                                                                                         \
    UT_array *array;                                                                               \
    type *ptr;                                                                                     \
  }

#define core_array_alloc(name, num)                                                                \
  do {                                                                                             \
    UT_icd icd = (UT_icd){sizeof(*((name).ptr)), NULL, NULL, NULL};                                \
    utarray_new((name).array, &icd);                                                               \
    utarray_resize((name).array, (num));                                                           \
    (name).ptr = utarray_front((name).array);                                                      \
  } while (0)

#define core_array_dealloc(name)                                                                   \
  do {                                                                                             \
    utarray_free((name).array);                                                                    \
  } while (0)

#define core_array_count(name) utarray_len((name).array)

#define _core_array_iter MACRO_CONCAT_LINE(core_array_iter)
#define core_array_foreach(name, ptrDecl, block)                                                   \
  do {                                                                                             \
    for (size_t _core_array_iter = 0; _core_array_iter < utarray_len((name).array);                \
         ++_core_array_iter) {                                                                     \
      ptrDecl = &(name).ptr[_core_array_iter];                                                      \
      block                                                                                        \
    }                                                                                              \
  } while (0)

#endif /* !CORE_ALLOC_H */

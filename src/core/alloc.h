/* Basic allocation. */

#ifndef CORE_ALLOC_H
#define CORE_ALLOC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include "junk.h"

void *core_memdup(const void *src, size_t size);

#endif /* !CORE_ALLOC_H */

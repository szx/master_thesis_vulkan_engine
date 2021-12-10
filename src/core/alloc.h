/* Basic allocation. Type info using codegen. */

#ifndef CORE_ALLOC_H
#define CORE_ALLOC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

/// struct foo {}; -> extern core_type_info *foo_type_info;
typedef struct core_type_info {
  const char *name;
  const size_t size;
  // TODO: Pointer to free functions()?
} core_type_info;
#define STRUCT(type) extern const core_type_info type##_type_info;
#include "../codegen/struct.def"
#undef STRUCT

#if defined(DEBUG)
typedef struct core_type_alloc_stats {
  size_t allocNum;
  size_t freeNum;
} core_type_alloc_stats;
#define STRUCT(type) extern core_type_alloc_stats type##_alloc_stats;
#include "../codegen/struct.def"
#undef STRUCT
#endif

/// Returns newly allocated memory for struct or NULL.
/// typeInfo should be only one of global constants declared above.
void *core_alloc_struct(const core_type_info *typeInfo);

/// Frees memory allocated for struct.
/// Returns pointer to freed memory or NULL if error.
/// typeInfo should be only one of global constants declared above.
void *core_free_struct(void *memory, const core_type_info *typeInfo);

#endif /* !CORE_ALLOC_H */

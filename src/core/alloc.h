/* Basic allocation. Type info using codegen. */

#ifndef CORE_ALLOC_H
#define CORE_ALLOC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(DEBUG)
/// struct foo {}; -> extern core_type_alloc_stats foo_alloc_stats;
typedef struct core_type_alloc_stats {
  uint64_t allocNum;
} core_type_alloc_stats;
#define STRUCT(type) extern core_type_alloc_stats type##_alloc_stats;
#include "../codegen/meta.def"
#undef STRUCT
#endif

/// struct foo {}; -> extern core_type_info foo_type_info;
typedef struct core_type_info {
  const char *name;
  const size_t size;
  void (*const free)(void *self);
#if defined(DEBUG)
  core_type_alloc_stats *allocStats;
#endif
} core_type_info;
#define STRUCT(type) extern const core_type_info type##_type_info;
#include "../codegen/meta.def"
#undef STRUCT

/// Header placed before pointer to struct allocated using core_alloc_struct.
typedef struct core_alloc_struct_header {
  const core_type_info *const typeInfo;
  // TODO: Object pools.
} core_alloc_struct_header;

/// Returns newly allocated memory for struct or NULL.
/// typeInfo should be only one of global constants declared above.
void *core_alloc_struct(const core_type_info *typeInfo);

/// Frees memory allocated for struct after calling appropriate user-defined free function.
/// Returns pointer to freed memory or NULL if error.
void *core_free_struct(void *memory);

// Logs debug info about all allocations.
void core_alloc_debug_print();

#endif /* !CORE_ALLOC_H */

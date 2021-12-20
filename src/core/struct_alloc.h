/* Type info using codegen. */

#ifndef STRUCT_ALLOC_H
#define STRUCT_ALLOC_H

#include "alloc.h"

#if defined(CODEGEN)
#define alloc_struct(type) (type *)malloc(sizeof(type))
#define alloc_struct_array(type, count) (type *)malloc(count * sizeof(type *))
#define count_struct_array(ptr) 1
#define init_struct_array(ptr) (void)(ptr)
#define init_struct_array_elem(ptr, i, initFunc, ...) (void)(ptr)
#define init_struct(ptr, initFunc, ...)                                                            \
  do {                                                                                             \
    (initFunc)((ptr), __VA_ARGS__);                                                                  \
  } while (0)
#define deinit_struct(ptr)
#define dealloc_struct(ptr) free(ptr)

#else
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
  void (*const deinit)(void *self);
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
  size_t count;
  bool initialized;
  // TODO: Object pools.
  // TODO: str.
  char padding[8];
} core_alloc_struct_header;

static_assert((sizeof(core_alloc_struct_header) % alignof(max_align_t)) == 0,
              "(sizeof(core_alloc_struct_header) % alignof(max_align_t)) != 0");

/// Returns header of allocated struct.
/// Panics if there is no header (can happen if struct is on stack).
core_alloc_struct_header *core_alloc_struct_header_get(void *memory);

/// Marks struct as initialized.
void core_alloc_struct_header_init(void *memory);

/// Returns newly allocated memory for struct or NULL.
/// typeInfo should be only one of global constants declared above.
void *core_alloc_struct(const core_type_info *typeInfo, size_t count);

/// Calls appropriate user-defined deinit function if struct is still initialized.
/// Iterates through struct arrays.
void core_deinit_struct(void *memory);

/// Frees memory allocated for struct.
/// Calls core_deinit_struct().
/// Returns pointer to freed memory or NULL if error.
void *core_dealloc_struct(void *memory);

// Logs debug info about all allocations.
void core_alloc_debug_print();

/// Use count_struct_array macro.
/// Returns number of allocated structs in struct array.
size_t platform_alloc_struct_array_count(void *memory);

/// Use init_struct macro.
/// Marks struct as initialized.
void platform_alloc_struct_mark_init(void *memory);

/// Use alloc_struct macro.
/// Returns newly allocated memory.
/// Initializes struct.
void *platform_alloc_struct(const core_type_info *typeInfo, size_t count);

/// Calls appropriate user-defined deinit function if struct is still initialized.
void platform_deinit_struct(void *memory);

/// Use dealloc_struct macro.
/// Frees memory allocated for struct after calling appropriate user-defined deinit function.
/// Sets memory pointer to NULL.
void platform_dealloc_struct(void **memory);

// Logs debug info about all allocations.
void platform_alloc_debug_print();

#define alloc_struct(type)                                                                         \
  (debug_msg("alloc_struct"), (type *)platform_alloc_struct(&type##_type_info, 1))
#define alloc_struct_array(type, count)                                                            \
  (debug_msg("alloc_struct_array"), (type *)platform_alloc_struct(&type##_type_info, (count)))
#define count_struct_array(ptr)                                                                    \
  (debug_msg("count_struct_array"), platform_alloc_struct_array_count((void *)(ptr)))
#define init_struct_array(ptr)                                                                     \
  do {                                                                                             \
    debug_msg("init_struct_array");                                                                \
    platform_alloc_struct_mark_init((ptr));                                                        \
  } while (0)
#define init_struct_array_elem(ptr, i, initFunc, ...)                                              \
  do {                                                                                             \
    debug_msg("init_struct_array_elem");                                                           \
    ((initFunc)(&(ptr)[(i)], ##__VA_ARGS__));                                                      \
    platform_alloc_struct_mark_init((ptr));                                                        \
  } while (0)
#define init_struct(ptr, initFunc, ...)                                                            \
  do {                                                                                             \
    debug_msg("init_struct");                                                                      \
    ((initFunc)((ptr), ##__VA_ARGS__));                                                            \
    platform_alloc_struct_mark_init((ptr));                                                        \
  } while (0)
#define deinit_struct(ptr) (debug_msg("deinit_struct"), platform_deinit_struct((void *)ptr))
#define dealloc_struct(ptr) (debug_msg("dealloc_struct"), platform_dealloc_struct((void *)&ptr))
#endif

#endif /* !PLATFORM_H */

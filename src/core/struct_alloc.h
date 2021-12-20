/* Basic cross-platform functions. */

#ifndef STRUCT_ALLOC_H
#define STRUCT_ALLOC_H

#include "alloc.h"

#if defined(CODEGEN)
#define alloc_struct(type) (type *)malloc(sizeof(type *))
#define alloc_struct_array(type, count) (type *)malloc(count * sizeof(type *))
#define count_struct_array(ptr) 1
#define init_struct_array(ptr) (void)(ptr)
#define init_struct_array_elem(ptr, i, initFunc, ...) (void)(ptr)
#define init_struct(ptr, initFunc, ...)                                                            \
  do {                                                                                             \
    (initFunc)(ptr, __VA_ARGS__);                                                                  \
  } while (0)
#define deinit_struct(ptr)
#define dealloc_struct(ptr) free(ptr)

#else
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
    ((initFunc)(&(ptr)[(i)], __VA_ARGS__));                                                        \
    platform_alloc_struct_mark_init((ptr));                                                        \
  } while (0)
#define init_struct(ptr, initFunc, ...)                                                            \
  do {                                                                                             \
    debug_msg("init_struct");                                                                      \
    ((initFunc)((ptr), __VA_ARGS__));                                                              \
    platform_alloc_struct_mark_init((ptr));                                                        \
  } while (0)
#define deinit_struct(ptr) (debug_msg("deinit_struct"), platform_deinit_struct((void *)ptr))
#define dealloc_struct(ptr) (debug_msg("dealloc_struct"), platform_dealloc_struct((void *)&ptr))
#endif

#endif /* !PLATFORM_H */

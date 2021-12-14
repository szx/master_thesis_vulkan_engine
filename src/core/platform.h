/* Basic cross-platform functions. Third-party libraries. */

#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__linux) || defined(__linux__) || defined(linux)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#endif

#ifndef SRC_PATH
#error "SRC_PATH should be defined by CMake"
#define SRC_PATH "SRC_PATH should be defined by CMake"
#endif
#ifndef VULKAN_HEADER_PATH
#error "VULKAN_HEADER_PATH should be defined by CMake"
#define VULKAN_HEADER_PATH "VULKAN_HEADER_PATH should be defined by CMake"
#endif

#include "alloc.h"

static_assert(sizeof(char) == sizeof(uint8_t), "sizeof(char) != sizeof(uint8_t)");

/// Allocates and initializes all global state used by functions below.
void platform_init();

/// Frees all global state used by functions below.
void platform_free();

/// Adds log.txt file to logger.
void log_init();

/// Closes log.txt file.
void log_free();

#if defined(DEBUG)
#define debug_msg(msg) log_debug("debug_msg: %s in %s:%s:%d", msg, __func__, __FILE__, __LINE__)
#else
#define debug_msg(msg) 0
#endif

/// Shows message dialog and exits.
void panic(const char *format, ...);

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

#define verify(cond)                                                                               \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      panic("%s:%d: %s failed", __FILE__, __LINE__, #cond);                                        \
    }                                                                                              \
  } while (0)

#define alloc_struct(type)                                                                         \
  (debug_msg("alloc_struct"), (type *)platform_alloc_struct(&type##_type_info, 1))
#define alloc_struct_array(type, count)                                                            \
  (debug_msg("alloc_struct_array"), (type *)platform_alloc_struct(&type##_type_info, (count)))
#define count_struct_array(ptr)                                                                    \
  (debug_msg("count_struct_array"), platform_alloc_struct_array_count((void *)(ptr)))
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

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* file system */
typedef struct platform_path {
  str data;
} platform_path;

platform_path platform_path_init(const char *data);
void platform_path_free(platform_path *self);
platform_path platform_path_copy(platform_path *self);
const char *platform_path_c_str(platform_path *self);

void platform_path_append(platform_path *self, const char *dirOrFileName);
void platform_path_append_ext(platform_path *self, const char *ext);
bool platform_path_equals(platform_path *self, platform_path *other);
bool platform_path_dirname_equals(platform_path *self, platform_path *other);
bool platform_path_ext_equals(platform_path *self, const char *ext);
platform_path platform_path_get_dirname(platform_path *self);
str platform_path_get_basename(platform_path *self);

#define T platform_path
#include "lst.h" // lst_platform_path

/// Returns new path to directory with current executable.
platform_path get_executable_dir_path();

/// Returns new path to file in specified directory.
platform_path get_executable_dir_file_path(const char *dirName, const char *fileName);

/// Returns new path to file in assets directory.
platform_path get_asset_file_path(const char *dirName, const char *fileName);

/// Returns new list of child paths.
lst_platform_path get_dir_children(platform_path *dirPath);

/// Returns null-terminated string with text file content.
char *read_text_file(platform_path *path, size_t *sourceLength);

/// Writes text file.
void write_text_file(platform_path *path, str *content);

#endif /* !PLATFORM_H */

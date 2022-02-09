/* Basic cross-platform functions. */
#pragma once

#ifndef SRC_PATH
#error "SRC_PATH should be defined by CMake"
#define SRC_PATH "SRC_PATH should be defined by CMake"
#endif
#ifndef VULKAN_HEADER_PATH
#error "VULKAN_HEADER_PATH should be defined by CMake"
#define VULKAN_HEADER_PATH "VULKAN_HEADER_PATH should be defined by CMake"
#endif

#include "alloc.h"
#include "junk.h"
#include "log.h"

#include "../codegen/globals.h"
#include "../codegen/meta.h"

/// Allocates and initializes all global state used by functions below.
void platform_create();

/// Frees all global state used by functions below.
void platform_destroy();

/// Adds log.txt file to logger.
void log_create();

/// Closes log.txt file.
void log_destroy();

/// Shows message dialog and exits.
void panic(const char *format, ...);

#define verify(cond)                                                                               \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      panic("%s:%d: %s failed", __FILE__, __LINE__, #cond);                                        \
    }                                                                                              \
  } while (0)

/* file system */
/// Checks if path extension (with dot) is equal to ext.
bool path_ext_equal(UT_string *path, const char *ext);

/// Returns new path to directory with current executable.
UT_string *get_executable_dir_path();

/// Returns new path to directory name.
UT_string *get_path_dirname(UT_string *path);

/// Appends new file/directory name to path.
void append_to_path(UT_string *path, const char *name);

/// Returns new path to file in specified directory.
UT_string *get_executable_dir_file_path(const char *dirName, const char *fileName);

/// Returns new path to file in assets directory.
UT_string *get_asset_file_path(const char *dirName, const char *fileName);

/// Returns new string with text file content.
UT_string *read_text_file(UT_string *path);

/// Writes text file.
bool write_text_file(UT_string *path, UT_string *content);

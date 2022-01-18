/* Basic conveniences. */
#pragma once

#if defined(__linux) || defined(__linux__) || defined(linux)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#endif

static_assert(sizeof(char) == sizeof(uint8_t), "sizeof(char) != sizeof(uint8_t)");

#define is_aligned(ptr, bytes) (((uintptr_t)(const void *)(ptr)) % (bytes) == 0)
#define member_size(type, member) sizeof(((type *)0)->member)
#define array_size(array) (sizeof(array) / sizeof((array)[0]))
#define utarray_size(array) ((array)->icd.sz * utarray_len((array)))

#if defined(DEBUG)
#define debug_msg(msg) log_trace("debug_msg: %s in %s:%s:%d", msg, __func__, __FILE__, __LINE__)
#else
void dummy_func();
#define debug_msg(msg) dummy_func()
#endif

/// Strip left whitespaces.
void strlstrip(char **str);
/// Strip right whitespaces.
/// NOTE: Replaces right whitespaces with '\0'.
void strrstrip(char **str);
/// Strip left and right whitespaces.
/// NOTE: Replaces right whitespaces with '\0'.
void strstrip(char **str);

#define MACRO_CONCAT_RAW(x, y) x##y
#define MACRO_CONCAT_EXPANDED(x, y) MACRO_CONCAT_RAW(x, y)
#define MACRO_CONCAT_LINE(x) MACRO_CONCAT_EXPANDED(x, __LINE__)
#define MACRO_EXPAND(...) __VA_ARGS__

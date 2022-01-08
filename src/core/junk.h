/* Basic conveniences. */
#pragma once

static_assert(sizeof(char) == sizeof(uint8_t), "sizeof(char) != sizeof(uint8_t)");

#define is_aligned(ptr, bytes) (((uintptr_t)(const void *)(ptr)) % (bytes) == 0)
#define member_size(type, member) sizeof(((type *)0)->member)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#if defined(DEBUG)
#define debug_msg(msg) log_trace("debug_msg: %s in %s:%s:%d", msg, __func__, __FILE__, __LINE__)
#else
void dummy_debug_msg();
#define debug_msg(msg) dummy_debug_msg()
#endif

/// Strip left whitespaces.
void lstrip(char **str);
/// Strip right whitespaces.
/// Note: Replaces right whitespaces with '\0'.
void rstrip(char **str);

#define MACRO_CONCAT_RAW(x, y) x##y
#define MACRO_CONCAT_EXPANDED(x, y) MACRO_CONCAT_RAW(x, y)
#define MACRO_CONCAT_LINE(x) MACRO_CONCAT_EXPANDED(x, __LINE__)

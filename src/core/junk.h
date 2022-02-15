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
#define utarray_eltsize(array) ((array)->icd.sz)

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

typedef XXH64_hash_t hash_t;
#define HASH_START(_state)                                                                         \
  XXH64_state_t *const _state = XXH64_createState();                                               \
  assert(_state != NULL);                                                                          \
  const XXH64_hash_t _state##seed = 0;                                                             \
  assert(XXH64_reset(_state, _state##seed) != XXH_ERROR);
#define HASH_UPDATE(_state, _dataPtr, _size)                                                       \
  assert(XXH64_update(_state, (const void *)_dataPtr, _size) != XXH_ERROR);
#define HASH_DIGEST(_state, _var) _var = XXH64_digest(_state);
#define HASH_END(_state) XXH64_freeState(_state);

#define INDENT_FORMAT_STRING "%*s"
#define INDENT_FORMAT_ARGS(_inc) (indent + _inc), ""
#define MAT4_FORMAT_STRING(_separator)                                                             \
  "%f %f %f %f" _separator "%f %f %f %f" _separator "%f %f %f %f" _separator "%f %f %f %f"
#define MAT4_FORMAT_ARGS(_prefix)                                                                  \
  _prefix[0][0], _prefix[0][1], _prefix[0][2], _prefix[0][3], _prefix[1][0], _prefix[1][1],        \
      _prefix[1][2], _prefix[1][3], _prefix[2][0], _prefix[2][1], _prefix[2][2], _prefix[2][3],    \
      _prefix[3][0], _prefix[3][1], _prefix[3][2], _prefix[3][3]

#define utarray_foreach_elem_it(_type, _elem, _utarray)                                            \
  UT_array *_elem##_array = _utarray;                                                              \
  _type _elem = {0};                                                                               \
  assert(_elem##_array->icd.sz == sizeof(*_elem));                                                 \
  while ((_elem = utarray_next(_elem##_array, _elem)))

#define utarray_foreach_elem_deref(_type, _elem, _utarray)                                         \
  UT_array *_elem##_array = _utarray;                                                              \
  _type *_elem##It = {0};                                                                          \
  _type _elem = {0};                                                                               \
  assert(_elem##_array->icd.sz == sizeof(_elem));                                                  \
  while ((_elem##It = utarray_next(_elem##_array, _elem##It)))                                     \
    if ((_elem = *_elem##It), true)

#define dl_foreach_elem(_type, _elem, _dl)                                                         \
  _type _elem = {0};                                                                               \
  _type _elem##Temp = {0};                                                                         \
  DL_FOREACH_SAFE(_dl, _elem, _elem##Temp)

/* macro magic */
// https://embeddedartistry.com/blog/2020/07/27/exploiting-the-preprocessor-for-fun-and-profit/

#define MACRO_CONCAT_RAW(x, y) x##y
#define MACRO_CONCAT(x, y) MACRO_CONCAT_RAW(x, y)
#define MACRO_CONCAT_LINE(x) MACRO_CONCAT(x, __LINE__)

#define MACRO_EMPTY()
#define MACRO_EXPAND(...) __VA_ARGS__
#define MACRO_DEFER(id) id MACRO_EMPTY()

// Returns number of variadic arguments.
// FIXME: Expands to 1 for 0 arguments.
#define MACRO_NARGS(...) MACRO_NARGS_(__VA_ARGS__, MACRO_NARGS_RSEQ_N())
#define MACRO_NARGS_(...) MACRO_ARG_N(__VA_ARGS__)
#define MACRO_NARGS_RSEQ_N() 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define MACRO_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N

// Returns name concatenated with number of variadic arguments.
#define MACRO_OVERLOAD(_name, ...) MACRO_CONCAT(_name, MACRO_NARGS(__VA_ARGS__))

// Foreach for variadic arguments.
#define MACRO_FOREACH(_macro, ...)                                                                 \
  MACRO_FOREACH_(MACRO_OVERLOAD(MACRO_FOREACH_, __VA_ARGS__), _macro, __VA_ARGS__)
#define MACRO_FOREACH_(_foreach, ...) _foreach(__VA_ARGS__)
#define MACRO_FOREACH_0(_macro)
#define MACRO_FOREACH_1(_macro, _elem) _macro(0, _elem) MACRO_FOREACH_0(_macro)
#define MACRO_FOREACH_2(_macro, _elem, ...) _macro(1, _elem) MACRO_FOREACH_1(_macro, __VA_ARGS__)
#define MACRO_FOREACH_3(_macro, _elem, ...) _macro(2, _elem) MACRO_FOREACH_2(_macro, __VA_ARGS__)
#define MACRO_FOREACH_4(_macro, _elem, ...) _macro(3, _elem) MACRO_FOREACH_3(_macro, __VA_ARGS__)
#define MACRO_FOREACH_5(_macro, _elem, ...) _macro(4, _elem) MACRO_FOREACH_4(_macro, __VA_ARGS__)
#define MACRO_FOREACH_6(_macro, _elem, ...) _macro(5, _elem) MACRO_FOREACH_5(_macro, __VA_ARGS__)
#define MACRO_FOREACH_7(_macro, _elem, ...) _macro(6, _elem) MACRO_FOREACH_6(_macro, __VA_ARGS__)
#define MACRO_FOREACH_8(_macro, _elem, ...) _macro(7, _elem) MACRO_FOREACH_7(_macro, __VA_ARGS__)
#define MACRO_FOREACH_9(_macro, _elem, ...) _macro(8, _elem) MACRO_FOREACH_8(_macro, __VA_ARGS__)
#define MACRO_FOREACH_10(_macro, _elem, ...) _macro(9, _elem) MACRO_FOREACH_9(_macro, __VA_ARGS__)
#define MACRO_FOREACH_11(_macro, _elem, ...) _macro(10, _elem) MACRO_FOREACH_10(_macro, __VA_ARGS__)
#define MACRO_FOREACH_12(_macro, _elem, ...) _macro(11, _elem) MACRO_FOREACH_11(_macro, __VA_ARGS__)
#define MACRO_FOREACH_13(_macro, _elem, ...) _macro(12, _elem) MACRO_FOREACH_12(_macro, __VA_ARGS__)
#define MACRO_FOREACH_14(_macro, _elem, ...) _macro(13, _elem) MACRO_FOREACH_13(_macro, __VA_ARGS__)
#define MACRO_FOREACH_15(_macro, _elem, ...) _macro(14, _elem) MACRO_FOREACH_14(_macro, __VA_ARGS__)
#define MACRO_FOREACH_16(_macro, _elem, ...) _macro(15, _elem) MACRO_FOREACH_15(_macro, __VA_ARGS__)

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

/// Strip left whitespaces.
void strlstrip(char **str);

/// Strip right whitespaces.
/// NOTE: Replaces right whitespaces with '\0'.
void strrstrip(char **str);

/// Strip left and right whitespaces.
/// NOTE: Replaces right whitespaces with '\0'.
void strstrip(char **str);

/// Count bits. Useful for flags.
size_t count_bits(size_t bits);

typedef XXH64_hash_t hash_t;
#define HASH_START(_state)                                                                         \
  XXH64_state_t *const _state = XXH64_createState();                                               \
  const XXH64_hash_t _state##seed = 0;                                                             \
  XXH64_reset(_state, _state##seed);
#define HASH_UPDATE(_state, _dataPtr, _size) XXH64_update(_state, (const void *)_dataPtr, _size);
#define HASH_DIGEST(_state, _var) _var = XXH64_digest(_state);
#define HASH_END(_state) XXH64_freeState(_state);

#define INDENT_FORMAT_STRING "%*s"
#define INDENT_FORMAT_ARGS(_inc) (indent + _inc), ""
#define VEC4_FORMAT_STRING() "%f %f %f %f"
#define VEC4_FORMAT_ARGS(_prefix) _prefix[0], _prefix[1], _prefix[2], _prefix[3]
#define MAT4_FORMAT_STRING(_separator)                                                             \
  VEC4_FORMAT_STRING()                                                                             \
  _separator VEC4_FORMAT_STRING() _separator VEC4_FORMAT_STRING() _separator VEC4_FORMAT_STRING()
#define MAT4_FORMAT_ARGS(_prefix)                                                                  \
  VEC4_FORMAT_ARGS(_prefix[0]), VEC4_FORMAT_ARGS(_prefix[1]), VEC4_FORMAT_ARGS(_prefix[2]),        \
      VEC4_FORMAT_ARGS(_prefix[3])

/* macro magic */
// https://embeddedartistry.com/blog/2020/07/27/exploiting-the-preprocessor-for-fun-and-profit/

#define MACRO_CONCAT_RAW(x, y) x##y
#define MACRO_CONCAT(x, y) MACRO_CONCAT_RAW(x, y)
#define MACRO_CONCAT_LINE(x) MACRO_CONCAT(x, __LINE__)

#define MACRO_EMPTY()
#define MACRO_EXPAND(...) __VA_ARGS__
#define MACRO_DEFER(id) id MACRO_EMPTY()

#define MACRO_DECL_AS_FOR_NEW(_name) for (int _name = 0; _name == 0; _name++)
#define MACRO_DECL_AS_FOR(_name, _decl) for (_decl; _name == 0; _name++)

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

#define is_aligned(ptr, bytes) (((uintptr_t)(const void *)(ptr)) % (bytes) == 0)
#define TYPEOF(x) __typeof__(x)
#define member_size(type, member) sizeof(((type *)0)->member)
#define array_size(array) (sizeof(array) / sizeof((array)[0]))
#define VLA(_count) ((_count) > 0 ? (_count) : 1)
#define utarray_size(array) ((array)->icd.sz * utarray_len((array)))
#define utarray_eltsize(array) ((array)->icd.sz)

#define utarray_foreach_elem_it(_type, _elem, _utarray)                                            \
  MACRO_DECL_AS_FOR_NEW(i)                                                                         \
  MACRO_DECL_AS_FOR(i, UT_array *_elem##_array = _utarray)                                         \
  MACRO_DECL_AS_FOR(i, _type _elem = {0})                                                          \
  while ((_elem = utarray_next(_elem##_array, _elem)))

#define utarray_foreach_elem_deref(_type, _elem, _utarray)                                         \
  MACRO_DECL_AS_FOR_NEW(_elem##i)                                                                  \
  MACRO_DECL_AS_FOR(_elem##i, UT_array *_elem##_array = _utarray)                                  \
  MACRO_DECL_AS_FOR(_elem##i, _type *_elem##It = {0})                                              \
  while ((_elem##It = utarray_next(_elem##_array, _elem##It)))                                     \
    MACRO_DECL_AS_FOR_NEW(_elem##j)                                                                \
  MACRO_DECL_AS_FOR(_elem##j, _type _elem = *_elem##It)

#define uthash_foreach_elem_it(_type, _elem, _uthash)                                              \
  MACRO_DECL_AS_FOR_NEW(_elem##i)                                                                  \
  MACRO_DECL_AS_FOR(_elem##i, _type _elem##_hash = _uthash)                                        \
  MACRO_DECL_AS_FOR(_elem##i, _type _elem = {0})                                                   \
  MACRO_DECL_AS_FOR(_elem##i, _type _elem##_temp = {0})                                            \
  HASH_ITER(hh, _elem##_hash, _elem, _elem##_temp)

#define dl_foreach_elem(_type, _elem, _dl)                                                         \
  MACRO_DECL_AS_FOR_NEW(_elem##i)                                                                  \
  MACRO_DECL_AS_FOR(_elem##i, _type _elem = {0})                                                   \
  MACRO_DECL_AS_FOR(_elem##i, _type _elem##Temp = {0})                                             \
  DL_FOREACH_SAFE(_dl, _elem, _elem##Temp)

// NOTE: Following linked list functions are not really speed demons.
size_t _dl_count(size_t offsetofNext, void *dl);
#define dl_count(_dl) _dl_count(offsetof(TYPEOF(*_dl), next), _dl)

void *_dl_elt(size_t offsetofNext, void *dl, size_t i);
#define dl_elt(_dl, _i) (TYPEOF(_dl)) _dl_elt(offsetof(TYPEOF(*_dl), next), _dl, _i)

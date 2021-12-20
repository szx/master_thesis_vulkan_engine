/* Basic conveniences. */

#pragma once

static_assert(sizeof(char) == sizeof(uint8_t), "sizeof(char) != sizeof(uint8_t)");

#define is_aligned(ptr, bytes) (((uintptr_t)(const void *)(ptr)) % (bytes) == 0)
#define member_size(type, member) sizeof(((type *)0)->member)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#include "alloc.h"

// Include all headers with struct definitions.
// TODO: Is there better way to get structs' sizeof?
#define PARENT
#include "../codegen/header.def"
#undef PARENT

#define STRUCT(type) const core_type_info type##_type_info = {#type, sizeof(type)};
#include "../codegen/struct.def"
#undef STRUCT

#if defined(DEBUG)
#define STRUCT(type) core_type_alloc_stats type##_alloc_stats = {0, 0};
#include "../codegen/struct.def"
#undef STRUCT
#endif

void *core_alloc_struct(const core_type_info *typeInfo) {
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  if (typeInfo == &type##_type_info) {                                                             \
    type##_alloc_stats.allocNum += 1;                                                              \
  }
#include "../codegen/struct.def"
#undef STRUCT
#endif

#define STRUCT(type)                                                                               \
  if (typeInfo == &type##_type_info) {                                                             \
    return malloc(type##_type_info.size);                                                          \
  }
#include "../codegen/struct.def"
#undef STRUCT

  return NULL;
}

void *core_free_struct(void *memory, const core_type_info *typeInfo) {
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  if (typeInfo == &type##_type_info) {                                                             \
    type##_alloc_stats.freeNum += 1;                                                               \
  }
#include "../codegen/struct.def"
#undef STRUCT
#endif

#define STRUCT(type)                                                                               \
  if (typeInfo == &type##_type_info) {                                                             \
    free(memory);                                                                                  \
    return memory;                                                                                 \
  }
#include "../codegen/struct.def"
#undef STRUCT
  return NULL;
}

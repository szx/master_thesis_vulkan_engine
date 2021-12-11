#include "alloc.h"

// Include all headers with struct definitions.
// TODO: Is there better way to get structs' sizeof?
#define PARENT
#include "../codegen/header.def"
#undef PARENT

// Preprocess STRUCT_* and FUNC_* defines.
#include "../codegen/meta.def"

#if defined(DEBUG)
// Define global core_type_alloc_stats variables.
#define STRUCT(type) core_type_alloc_stats type##_alloc_stats = {0};
#include "../codegen/meta.def"
#endif

// Define global core_type_info constants.
void dummy_struct_free(void *self) {}
#define FREE_FUNC_PTR(type) (void(*)(void *)) & type##_free
#define STRUCT_COND(type) FUNC_##type##_free
#define STRUCT_COND_TRUE(type) static void (*const type##_free_impl)(void *) = FREE_FUNC_PTR(type);
#define STRUCT_COND_FALSE(type) static void (*const type##_free_impl)(void *) = &dummy_struct_free;
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  const core_type_info type##_type_info = {#type, sizeof(type), type##_free_impl,                  \
                                           &type##_alloc_stats};
#else
#define STRUCT(type)                                                                               \
  const core_type_info type##_type_info = {#type, sizeof(type), type##_free_impl};
#endif
#include "../codegen/meta.def"

void *core_alloc_struct(const core_type_info *typeInfo) {
#if defined(DEBUG)
  typeInfo->allocStats->allocNum += 1;
#endif
  return malloc(typeInfo->size);
}

void *core_free_struct(void *memory, const core_type_info *typeInfo) {
#if defined(DEBUG)
  typeInfo->allocStats->allocNum -= 1;
#endif
  typeInfo->free(memory);
  free(memory);
  return memory;
}

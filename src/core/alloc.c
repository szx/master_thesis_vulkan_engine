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

const core_alloc_struct_header *core_alloc_struct_header_get(void *memory) {
  core_alloc_struct_header *header =
      (core_alloc_struct_header *)((char *)memory - sizeof(core_alloc_struct_header));
  return header;
}

void *core_alloc_struct(const core_type_info *typeInfo, size_t count) {
  log_debug("alloc_struct %s: size=%d count=%d", typeInfo->name, typeInfo->size, count);
  size_t structSize = typeInfo->size * count;
  core_alloc_struct_header *header =
      (core_alloc_struct_header *)malloc(sizeof(core_alloc_struct_header) + structSize);
  if (header == NULL) {
    return NULL;
  }
#if defined(DEBUG)
  typeInfo->allocStats->allocNum += count;
#endif
  memcpy(header, &(core_alloc_struct_header){.typeInfo = typeInfo, .count = count},
         sizeof(core_alloc_struct_header));
  void *memory = (void *)((char *)header + sizeof(core_alloc_struct_header));
  return memory;
}

void *core_free_struct(void *memory) {
  core_alloc_struct_header *header =
      (core_alloc_struct_header *)((char *)memory - sizeof(core_alloc_struct_header));
  const core_type_info *typeInfo = header->typeInfo;
#if defined(DEBUG)
  typeInfo->allocStats->allocNum -= header->count;
#endif
  typeInfo->free(memory);
  free(header);
  return memory;
}
void core_alloc_debug_print() {
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  log_debug("alloc_debug_print: type_info %s size=%d free=%p", type##_type_info.name,              \
            type##_type_info.size, type##_type_info.free);
#include "../codegen/meta.def"
#define STRUCT(type)                                                                               \
  if (type##_alloc_stats.allocNum != 0) {                                                          \
    log_debug("alloc_debug_print: POSSIBLE MEMLEAK %s allocNum=%d", type##_type_info.name,         \
              type##_alloc_stats.allocNum);                                                        \
  }
#include "../codegen/meta.def"
#endif
}

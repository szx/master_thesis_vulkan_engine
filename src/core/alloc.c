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
  log_debug("alloc_struct %s: size=%d", typeInfo->name, typeInfo->size);
#if defined(DEBUG)
  typeInfo->allocStats->allocNum += 1;
#endif
  size_t structSize = typeInfo->size;
  core_alloc_struct_header *memoryWithHeader =
      (core_alloc_struct_header *)malloc(sizeof(core_alloc_struct_header) + structSize);
  if (memoryWithHeader == NULL) {
    return NULL;
  }
  memcpy(memoryWithHeader, &(core_alloc_struct_header){.typeInfo = typeInfo},
         sizeof(core_alloc_struct_header));
  void *memory = (void *)((char *)memoryWithHeader + sizeof(core_alloc_struct_header));
  return memory;
}

void *core_free_struct(void *memory) {
  core_alloc_struct_header *memoryWithHeader =
      (core_alloc_struct_header *)((char *)memory - sizeof(core_alloc_struct_header));
  const core_type_info *typeInfo = memoryWithHeader->typeInfo;
#if defined(DEBUG)
  typeInfo->allocStats->allocNum -= 1;
#endif
  typeInfo->free(memory);
  free(memoryWithHeader);
  return memory;
}
void core_alloc_debug_print() {
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  log_debug("alloc_debug_print:\ttype_info\t%s\tsize=%d\tfree=%p", type##_type_info.name,          \
            type##_type_info.size, type##_type_info.free);
#include "../codegen/meta.def"
#define STRUCT(type)                                                                               \
  if (type##_alloc_stats.allocNum != 0) {                                                          \
    log_debug("alloc_debug_print:\tPOSSIBLE MEMLEAK\t%s\tallocNum=%d", type##_type_info.name,      \
              type##_alloc_stats.allocNum);                                                        \
  }
#include "../codegen/meta.def"
#endif
}
#include "struct_alloc.h"

#if defined(CODEGEN)
// Target is codegen, do not expose struct alloc functionality.
#else

// Include all headers with struct definitions.
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
void dummy_struct_deinit(void *self) { assert(0); }
#define DEINIT_FUNC_PTR(type) (void(*)(void *)) & type##_deinit
#define STRUCT_COND(type) FUNC_##type##_deinit
#define STRUCT_COND_TRUE(type)                                                                     \
  static void (*const type##_deinit_impl)(void *) = DEINIT_FUNC_PTR(type);
#define STRUCT_COND_FALSE(type)                                                                    \
  static void (*const type##_deinit_impl)(void *) = &dummy_struct_deinit;
#if defined(DEBUG)
#define STRUCT(type)                                                                               \
  const core_type_info type##_type_info = {#type, sizeof(type), type##_deinit_impl,                \
                                           &type##_alloc_stats};
#else
#define STRUCT(type)                                                                               \
  const core_type_info type##_type_info = {#type, sizeof(type), type##_deinit_impl};
#endif
#include "../codegen/meta.def"

static bool is_header_magic_number_correct(core_alloc_struct_header *header) {
  return header->padding[0] == 'M' && header->padding[1] == 'A' && header->padding[2] == 'G' &&
         header->padding[3] == 'I' && header->padding[4] == 'C' && header->padding[5] == '1' &&
         header->padding[6] == '2' && header->padding[7] == '3';
}

core_alloc_struct_header *core_alloc_struct_header_get(void *memory) {
  core_alloc_struct_header *header =
      (core_alloc_struct_header *)((char *)memory - sizeof(core_alloc_struct_header));
  // Check if heap pointer was allocated using core_alloc_struct().
  verify(is_header_magic_number_correct(header));
  // TODO: Check if not stack pointer? Needs custom allocator. (or __builtin_frame_address?)
  return header;
}

void core_alloc_struct_header_init(void *memory) {
  core_alloc_struct_header *header = core_alloc_struct_header_get(memory);
  // TODO: verify(header->initialized == false);
  header->initialized = true;
}

void *core_alloc_struct(const core_type_info *typeInfo, size_t count) {
  log_debug("alloc_struct %s: size=%d count=%d", typeInfo->name, typeInfo->size, count);
  size_t headerSize = sizeof(core_alloc_struct_header);
  size_t structSize = typeInfo->size * count;
  size_t structPadding = 0;
  const size_t alignment = alignof(max_align_t);
  if ((structSize % alignment) != 0) {
    structPadding = (structSize / alignment);
  }
  size_t structAndHeaderSize = headerSize + structSize + structPadding;
  core_alloc_struct_header *header = (core_alloc_struct_header *)malloc(structAndHeaderSize);
  if (header == NULL) {
    return NULL;
  }
#if defined(DEBUG)
  typeInfo->allocStats->allocNum += count;
#endif
  memcpy(header,
         &(core_alloc_struct_header){.typeInfo = typeInfo, .count = count, .initialized = false},
         sizeof(core_alloc_struct_header));
  header->padding[0] = 'M';
  header->padding[1] = 'A';
  header->padding[2] = 'G';
  header->padding[3] = 'I';
  header->padding[4] = 'C';
  header->padding[5] = '1';
  header->padding[6] = '2';
  header->padding[7] = '3';
  void *memory = (void *)((char *)header + sizeof(core_alloc_struct_header));
  assert(is_aligned(header, alignment));
  assert(is_aligned(memory, alignment));
  return memory;
}

void core_deinit_struct(void *memory) {
  core_alloc_struct_header *header = core_alloc_struct_header_get(memory);
  if (header->initialized) {
    const core_type_info *typeInfo = header->typeInfo;
    char *it = memory;
    for (size_t i = 0; i < header->count; i++) {
      typeInfo->deinit(it);
      it += typeInfo->size;
    }
    header->initialized = false;
  }
}

void *core_dealloc_struct(void *memory) {
  core_alloc_struct_header *header = core_alloc_struct_header_get(memory);
  const core_type_info *typeInfo = header->typeInfo;
#if defined(DEBUG)
  typeInfo->allocStats->allocNum -= header->count;
#endif
  core_deinit_struct(memory);
  free(header);
  return memory;
}

void core_alloc_debug_print() {
#if defined(DEBUG)
/*#define STRUCT(type) \
  if (type##_type_info.deinit == &dummy_struct_deinit) {                                           \
    log_debug("alloc_debug_print: type_info %s size=%d deinit=DUMMY", type##_type_info.name,       \
              type##_type_info.size);                                                              \
  } else {                                                                                         \
    log_debug("alloc_debug_print: type_info %s size=%d deinit=FUNC %p", type##_type_info.name,     \
              type##_type_info.size, type##_type_info.deinit);                                     \
  }
#include "../codegen/meta.def"*/
#define STRUCT(type)                                                                               \
  if (type##_alloc_stats.allocNum != 0) {                                                          \
    log_debug("alloc_debug_print: POSSIBLE MEMLEAK %s allocNum=%d", type##_type_info.name,         \
              type##_alloc_stats.allocNum);                                                        \
  }
#include "../codegen/meta.def"
#endif
}

#endif

#if !defined(CODEGEN)
size_t platform_alloc_struct_array_count(void *memory) {
  if (memory == NULL) {
    return 0;
  }
  return core_alloc_struct_header_get((void *)(memory))->count;
}

void platform_alloc_struct_mark_init(void *memory) {
  if (memory == NULL) {
    return;
  }
  core_alloc_struct_header_init(memory);
}

void *platform_alloc_struct(const core_type_info *typeInfo, size_t count) {
  void *allocatedMemory = core_alloc_struct(typeInfo, count);
  assert(allocatedMemory != NULL);
  log_trace("alloc_struct: %p", allocatedMemory);
  return allocatedMemory;
}

void platform_deinit_struct(void *memory) {
  log_trace("deinit_struct: %p", memory);
  core_deinit_struct(memory);
}

void platform_dealloc_struct(void **memory) {
  log_trace("dealloc_struct: %p", *memory);
  if (*memory == NULL) {
    return;
  }
  void *memoryFreeResult = core_dealloc_struct(*memory);
  assert(memoryFreeResult == *memory);
  *memory = NULL;
}

void platform_alloc_debug_print() { core_alloc_debug_print(); }

#endif

#include "struct_alloc.h"

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

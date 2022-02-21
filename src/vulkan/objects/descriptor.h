/* C structs used by shaders in uniform buffers. */

#pragma once

#include "../../core/thirdparty.h"
#include "common.h"

typedef struct vulkan_transform_uniform_struct {
  mat4 viewMat;
  mat4 projMat;
} vulkan_transform_uniform_struct;

// _uniform_element
// HIRO _uniform_buffer
#include "../../codegen/descriptors.h"

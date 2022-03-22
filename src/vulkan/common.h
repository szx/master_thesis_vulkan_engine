/* Vulkan API helper objects. */

#pragma once

#include "../core/core.h"
#include "../data/data.h"
#include "constants.h"
#include "debug.h"
#include "functions.h"
#include "objects/device.h"

typedef enum vulkan_attribute_type {
  vulkan_attribute_type_unknown = 0,
  vulkan_attribute_type_position = 1 << 0,
  vulkan_attribute_type_normal = 1 << 1,
  vulkan_attribute_type_color = 1 << 2,
  vulkan_attribute_type_texcoord = 1 << 3,
  vulkan_attribute_type_tangent = 1 << 4,
} vulkan_attribute_type;

/* Mesh state.
 * Represents a group of primitives.
 * Used to:
 * store all geometry of one object,
 * calculate culling boundaries of objects.
 * TODO: skinning
 */
#pragma once

#include "common.h"

typedef struct asset_mesh {
  UT_array *primitives;             /// asset_primitive* array.
  VULKAN_ASSET_FIELD_DECLS(mesh)
} asset_mesh;

VULKAN_ASSET_FUNC_DECLS(mesh)

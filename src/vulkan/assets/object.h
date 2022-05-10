/* Object state.
 * Represents one object in scene.
 * Contains one mesh and transform.
 * Used by scene data.
 */
#pragma once

#include "camera.h"
#include "common.h"
#include "mesh.h"

typedef struct vulkan_asset_object {
  vulkan_asset_mesh *mesh;     /// Can be NULL if object is only transforms children.
  vulkan_asset_camera *camera; /// Can be NULL if object has no attached camera.
  mat4 transform;
  UT_array *children; /// vulkan_asset_object* array

  VULKAN_ASSET_FIELD_DECLS(object)
} vulkan_asset_object;

VULKAN_ASSET_FUNC_DECLS(object)

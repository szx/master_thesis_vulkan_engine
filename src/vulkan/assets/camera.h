/* Camera state.
 * Represents camera in scene.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "common.h"

typedef struct vulkan_asset_camera {
  vulkan_camera_type type;
  union {
    float fovY;        ///< Vertical field of view (radians).
    float aspectRatio; ///< Aspect ratio.
  };
  union {
    float magX; ///< Horizontal magnification of view.
    float magY; ///< Vertical magnification of view.
  };
  float nearZ; ///< Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;  ///< Max depth range: distance from far clipping plane (along +Z axis!)

  bool dirty; ///< True if camera state updated.
  VULKAN_ASSET_FIELD_DECLS(camera)
} vulkan_asset_camera;

VULKAN_ASSET_FUNC_DECLS(camera)
void vulkan_asset_camera_copy(vulkan_asset_camera *dst, vulkan_asset_camera *src);

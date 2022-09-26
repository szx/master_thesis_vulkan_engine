/* Camera state.
 * Represents camera in scene.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "common.h"

typedef struct asset_camera {
  camera_type type;
  union {
    struct {
      float fovY;        ///< Vertical field of view (radians).
      float aspectRatio; ///< Aspect ratio.
    };
    struct {
      float magX; ///< Horizontal magnification of view.
      float magY; ///< Vertical magnification of view.
    };
  };
  float nearZ; ///< Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;  ///< Max depth range: distance from far clipping plane (along +Z axis!)

  bool dirty; ///< True if camera state updated.
  VULKAN_ASSET_FIELD_DECLS(camera)
} asset_camera;

VULKAN_ASSET_FUNC_DECLS(camera)
void asset_camera_copy(asset_camera *dst, asset_camera *src);

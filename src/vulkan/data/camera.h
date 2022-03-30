/* Camera state.
 * Represents camera in scene.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "common.h"

typedef enum vulkan_camera_type {
  vulkan_camera_type_orthographic,
  vulkan_camera_type_perspective,
  vulkan_camera_type_count
} vulkan_camera_type;

typedef struct vulkan_data_camera {
  vulkan_camera_type type;
  union {
    float fovY; ///< Vertical field of view.
    float magX; ///< Horizontal magnification of view.
  };
  union {
    float aspectRatio; ///< Aspect ratio.
    float magY;        ///< Vertical magnification of view.
  };
  float nearZ; ///< Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;  ///< Max depth range: distance from far clipping plane (along +Z axis!)

  bool dirty; ///< True if camera state updated.
  DECL_VULKAN_ENTITY(camera)
} vulkan_data_camera;

void vulkan_data_camera_init(vulkan_data_camera *camera, vulkan_data_scene *sceneData);
void vulkan_data_camera_deinit(vulkan_data_camera *camera);
void vulkan_data_camera_copy(vulkan_data_camera *dst, vulkan_data_camera *src);

data_key vulkan_data_camera_calculate_key(vulkan_data_camera *camera);
void vulkan_data_camera_serialize(vulkan_data_camera *camera, data_asset_db *assetDb);
void vulkan_data_camera_deserialize(vulkan_data_camera *camera, data_asset_db *assetDb,
                                    data_key key);

void vulkan_data_camera_debug_print(vulkan_data_camera *camera, int indent);

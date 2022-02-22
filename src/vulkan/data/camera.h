/* Camera state.
 * Represents camera in scene.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "common.h"

typedef struct vulkan_data_camera {
  /* View matrix: camera position and rotation */
  vec3 position;   /// Position in world space.
  versor rotation; /// Rotation in world space.
  /* Projection matrix: view frustum */
  float fovY;        /// Vertical field of view.
  float aspectRatio; /// Aspect ratio.
  float nearZ;       /// Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;        /// Max depth range: distance from far clipping plane (along +Z axis!)

  bool dirty;    /// True if camera state updated.
  DECL_VULKAN_ENTITY(camera)
} vulkan_data_camera;

void vulkan_data_camera_init(vulkan_data_camera *camera, vulkan_data_scene *sceneData);
void vulkan_data_camera_deinit(vulkan_data_camera *camera);
void vulkan_data_camera_copy(vulkan_data_camera *dst, vulkan_data_camera *src);

void vulkan_data_camera_update_aspect_ratio(vulkan_data_camera *camera, float aspectRatio);

data_key vulkan_data_camera_calculate_key(vulkan_data_camera *camera);
void vulkan_data_camera_serialize(vulkan_data_camera *camera, data_asset_db *assetDb);
void vulkan_data_camera_deserialize(vulkan_data_camera *camera, data_asset_db *assetDb,
                                    data_key key);

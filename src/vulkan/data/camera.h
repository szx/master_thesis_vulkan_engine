/* Camera state.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_camera_data {
  /* View matrix: camera position and rotation */
  vec3 position;   /// Position in world space.
  versor rotation; /// Rotation in world space.
  /* Projection matrix: view frustum */
  float fovY;        /// Vertical field of view.
  float aspectRatio; /// Aspect ratio.
  float nearZ;       /// Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;        /// Max depth range: distance from far clipping plane (along +Z axis!)

  bool dirty;    /// True if camera state updated.
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_camera_data;

void vulkan_camera_data_init(vulkan_camera_data *camera);
void vulkan_camera_data_deinit(vulkan_camera_data *camera);

void vulkan_camera_data_update_aspect_ratio(vulkan_camera_data *camera, float aspectRatio);

data_key vulkan_camera_data_calculate_key(vulkan_camera_data *camera);
void vulkan_camera_data_serialize(vulkan_camera_data *camera, data_asset_db *assetDb);
void vulkan_camera_data_deserialize(vulkan_camera_data *camera, data_asset_db *assetDb,
                                    data_key key);

/* Camera state.
 * Used to generate view and projection matrices.
 */
#pragma once

#include "../core/platform.h"
#include "../data/data.h"
#include "functions.h"

/// Contains index buffer, interleaved vertex stream and topology of the part of the mesh.
typedef struct vulkan_camera {
  /* View matrix: camera position and rotation */
  vec3 position;   /// Position in world space.
  versor rotation; /// Rotation in world space.
  /* Projection matrix: view frustum */
  float fovY;        /// Vertical field of view.
  float aspectRatio; /// Aspect ratio.
  float nearZ;       /// Min depth range: distance from near clipping plane (along +Z axis!)
  float farZ;        /// Max depth range: distance from far clipping plane (along +Z axis!)
  /* state */
  bool dirty; /// True if camera state updated.
} vulkan_camera;

vulkan_camera *vulkan_camera_create();
void vulkan_camera_destroy(vulkan_camera *camera);

/// Updates aspect ratio after swap chain recreation.
void vulkan_camera_update_aspect_ratio(vulkan_camera *camera, float aspectRatio);

/// Returns data blob with newly allocated camera data.
data_blob vulkan_camera_serialize(vulkan_camera *camera);
// TODO: vulkan_camera_deserialize

void vulkan_camera_update_uniform_buffer_data(vulkan_camera *camera,
                                              vulkan_uniform_buffer *uniformBuffer);

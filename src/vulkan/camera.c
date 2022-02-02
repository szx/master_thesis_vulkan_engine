#include "camera.h"
#include "../data/db.h"

vulkan_camera *vulkan_camera_create() {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));
  glm_vec3((vec3){0.0f, 0.0f, -1.0f}, camera->position);
  glm_quat_identity(camera->rotation);
  camera->fovY = 90.0f;
  camera->aspectRatio = 1.0f;
  camera->nearZ = 0.1f;
  camera->farZ = 256.0f;
  camera->dirty = true;
  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) { core_free(camera); }

void vulkan_camera_update_aspect_ratio(vulkan_camera *camera, float aspectRatio) {
  camera->aspectRatio = aspectRatio;
  camera->dirty = true;
}

data_blob vulkan_camera_serialize(vulkan_camera *camera) {
  UT_array *array = NULL;
  utarray_alloc(array, sizeof(float));
  utarray_push_back(array, &(camera->position[0]));
  utarray_push_back(array, &(camera->position[1]));
  utarray_push_back(array, &(camera->position[2]));
  utarray_push_back(array, &(camera->rotation[0]));
  utarray_push_back(array, &(camera->rotation[1]));
  utarray_push_back(array, &(camera->rotation[2]));
  utarray_push_back(array, &(camera->rotation[3]));
  utarray_push_back(array, &(camera->fovY));
  utarray_push_back(array, &(camera->aspectRatio));
  utarray_push_back(array, &(camera->nearZ));
  utarray_push_back(array, &(camera->farZ));
  data_blob blob = {0};
  data_blob_init(&blob, array);
  return blob;
}

void vulkan_camera_deserialize(vulkan_camera *camera, data_blob blob) {
  UT_array *array = NULL;
  data_deserialize_blob((const uint8_t *)blob.value->d, utarray_size(blob.value), &array);
  float *ptr = (float *)utarray_front(array);
  camera->position[0] = ptr[0];
  camera->position[1] = ptr[1];
  camera->position[2] = ptr[2];
  camera->rotation[0] = ptr[3];
  camera->rotation[1] = ptr[4];
  camera->rotation[2] = ptr[5];
  camera->rotation[3] = ptr[6];
  camera->fovY = ptr[7];
  camera->aspectRatio = ptr[8];
  camera->nearZ = ptr[9];
  camera->farZ = ptr[10];
  utarray_free(array);
}

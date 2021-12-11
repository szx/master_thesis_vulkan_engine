/* Precompiled header for third-party libraries. */

#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan/vulkan.h"

#include <GLFW/glfw3.h>

#include "log.h"
#include "str.h"

#include "cglm/cglm.h"
#include "cgltf.h"
#include "stb_image.h"

#include <assert.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

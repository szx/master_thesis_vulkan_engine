/* Precompiled header for third-party libraries. */

#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan/vulkan.h"

#include <GLFW/glfw3.h>

#include "shaderc/shaderc.h"

#include "log.h"
#include "str.h"

#include "utlist.h"

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm/cglm.h"

#include "cgltf.h"
#include "stb_image.h"

#include <assert.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Precompiled header for third-party libraries. */

#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan/vulkan.h"

#include "meshoptimizer.h"
#include "shaderc/shaderc.h"
#include <sqlite3.h>

#include <GLFW/glfw3.h>

#include "log.h"

#include "utarray.h"
#include "uthash.h"
#include "utlist.h"
#include "utstring.h"

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#define CGLM_FORCE_LEFT_HANDED
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
#include <string.h>

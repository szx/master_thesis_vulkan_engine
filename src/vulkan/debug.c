#include "debug.h"

/// Creates persistent debug messenger.
static VkResult create_debug_utils_messenger_ext(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

/// Destroys persistent debug messenger.
static void destroy_debug_utils_messenger_ext(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                                                                 "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL) {
    func(instance, debugMessenger, pAllocator);
  }
}

static VKAPI_ATTR void
debug_callback_log_prepare(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, int *logLevel,
                           UT_string *msg) {
  (*logLevel) = LOG_FATAL;
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    (*logLevel) = LOG_DEBUG;
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    (*logLevel) = LOG_INFO;
  }
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    (*logLevel) = LOG_WARN;
  }
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    (*logLevel) = LOG_ERROR;
  }

  if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
    utstring_printf(msg, "GENERAL ");
  } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
    utstring_printf(msg, "VALIDATION ");
  } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
    utstring_printf(msg, "PERFORMANCE ");
  }

  utstring_printf(msg, "[%s]: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);

  if (pCallbackData->queueLabelCount > 0) {
    for (size_t i = 0; i < pCallbackData->queueLabelCount; i++) {
      const VkDebugUtilsLabelEXT *label = &pCallbackData->pQueueLabels[i];
      utstring_printf(msg, "\nQueueLabel %zd: %s (%f,%f,%f,%f);", i, label->pLabelName,
                      label->color[0], label->color[1], label->color[2], label->color[3]);
    }
  }
  if (pCallbackData->cmdBufLabelCount > 0) {
    for (size_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
      const VkDebugUtilsLabelEXT *label = &pCallbackData->pCmdBufLabels[i];
      utstring_printf(msg, "\nCmdBufLabel %zd: %s (%f,%f,%f,%f);", i, label->pLabelName,
                      label->color[0], label->color[1], label->color[2], label->color[3]);
    }
  }
  if (pCallbackData->objectCount > 0) {
    for (size_t i = 0; i < pCallbackData->objectCount; i++) {
      const VkDebugUtilsObjectNameInfoEXT *object = &pCallbackData->pObjects[i];
      utstring_printf(msg, "\nObject %zd: name = %s, handle = %zx, type = %s;", i,
                      object->pObjectName, object->objectHandle,
                      VkObjectType_debug_str(object->sType));
    }
  }
}

/// Persistent debug callback for Vulkan objects.
/// Logs output of validation layers.
/// Logs user data.
static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback_general(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  int logLevel;
  UT_string *msg;
  utstring_new(msg);

  debug_callback_log_prepare(messageSeverity, messageType, pCallbackData, &logLevel, msg);
  log_log(logLevel, __FILE__, __LINE__, "vulkan_debug_callback_general: %s", utstring_body(msg));
  utstring_free(msg);

  // TODO: debug callback user data
  return VK_FALSE; // True is reserved for layers.
}

/* public */

vulkan_debug *vulkan_debug_create(bool enabled, VkInstance instance,
                                  const VkAllocationCallbacks *vka) {
  vulkan_debug *debug = core_alloc(sizeof(vulkan_debug));
  debug->enabled = enabled;
  debug->instance = instance;
  debug->vka = vka;
  debug->debugMessenger = VK_NULL_HANDLE;
  if (debug->enabled) {
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
        vulkan_debug_messenger_create_info(debug_callback_general);
    verify(create_debug_utils_messenger_ext(debug->instance, &debugCreateInfo, debug->vka,
                                            &debug->debugMessenger) == VK_SUCCESS);
  }
  return debug;
}

void vulkan_debug_destroy(vulkan_debug *debug) {
  if (debug->enabled) {
    destroy_debug_utils_messenger_ext(debug->instance, debug->debugMessenger, NULL);
  }
  core_free(debug);
}

VkBool32 VKAPI_CALL vulkan_debug_callback_for_instance(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  int logLevel;
  UT_string *msg;
  utstring_new(msg);

  debug_callback_log_prepare(messageSeverity, messageType, pCallbackData, &logLevel, msg);
  log_log(logLevel, __FILE__, __LINE__, "vulkan_debug_callback_for_instance: %s",
          utstring_body(msg));
  utstring_free(msg);

  return VK_FALSE; // True is reserved for layers.
}

VkDebugUtilsMessengerCreateInfoEXT
vulkan_debug_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback) {
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
  debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debugCreateInfo.pfnUserCallback = pfnUserCallback;
  return debugCreateInfo;
}

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
                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                           log_level *logLevel, UT_string *msg) {
  (*logLevel) = log_level_fatal;
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    (*logLevel) = log_level_debug;
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    (*logLevel) = log_level_info;
  }
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    (*logLevel) = log_level_warn;
  }
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    (*logLevel) = log_level_error;
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
  log_level logLevel;
  UT_string *msg;
  utstring_new(msg);

  debug_callback_log_prepare(messageSeverity, messageType, pCallbackData, &logLevel, msg);
  log_pretty(logLevel, __func__, __FILE__, __LINE__, "vulkan_debug_callback_general: %s",
             utstring_body(msg));
  utstring_free(msg);

  // TODO: debug callback user data
  return VK_FALSE; // True is reserved for layers.
}

static void name_object(vulkan_debug *debug, VkObjectType objectType, uint64_t objectHandle,
                        const char *format, va_list args) {
  if (!debug->enabled) {
    return;
  }

  va_list copy;
  va_copy(copy, args);
  UT_string *s;
  utstring_new(s);
  utstring_printf_va(s, format, copy);
  va_end(copy);
  const char *objectName = core_strdup(utstring_body(s));
  utstring_free(s);

  vulkan_debug_name_data *nameData;
  HASH_FIND_STR(debug->names, objectName, nameData);
  if (nameData == NULL) {
    nameData = core_alloc(sizeof(vulkan_debug_name_data));
    nameData->name = objectName;
    HASH_ADD_KEYPTR(hh, debug->names, nameData->name, strlen(nameData->name), nameData);
  } else {
    free((char *)objectName);
  }

  VkDebugUtilsObjectNameInfoEXT nameInfo = {0};
  nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  nameInfo.objectType = objectType;
  nameInfo.objectHandle = objectHandle;
  nameInfo.pObjectName = nameData->name;

  // TODO: Add names and additional data to vulkan_debug.
  assert(debug->pDevice != VK_NULL_HANDLE);
  verify(debug->setDebugUtilsObjectNameEXT(*debug->pDevice, &nameInfo) == VK_SUCCESS);
}

/* public */

vulkan_debug *vulkan_debug_create(bool enabled, VkDevice *pDevice, VkInstance instance,
                                  const VkAllocationCallbacks *vka) {
  vulkan_debug *debug = core_alloc(sizeof(vulkan_debug));
  debug->enabled = enabled;
  debug->pDevice = pDevice;
  debug->instance = instance;
  debug->vka = vka;
  debug->debugMessenger = VK_NULL_HANDLE;
  if (debug->enabled) {
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo =
        vulkan_debug_messenger_create_info(debug_callback_general);
    verify(create_debug_utils_messenger_ext(debug->instance, &debugCreateInfo, debug->vka,
                                            &debug->debugMessenger) == VK_SUCCESS);
    debug->cmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        debug->instance, "vkCmdBeginDebugUtilsLabelEXT");
    debug->cmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        debug->instance, "vkCmdEndDebugUtilsLabelEXT");
    debug->cmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        debug->instance, "vkCmdInsertDebugUtilsLabelEXT");
    debug->setDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(
        debug->instance, "vkSetDebugUtilsObjectNameEXT");

    debug->names = NULL;
  }
  return debug;
}

void vulkan_debug_destroy(vulkan_debug *debug) {
  if (debug->enabled) {
    destroy_debug_utils_messenger_ext(debug->instance, debug->debugMessenger, NULL);
    vulkan_debug_name_data *nameData, *temp;
    HASH_ITER(hh, debug->names, nameData, temp) {
      HASH_DEL(debug->names, nameData);
      free((char *)nameData->name);
      core_free(nameData);
    }
  }
  core_free(debug);
}

#define VULKAN_DEBUG_NAME_FUNC_DEF(_funcName, _handleType, _objectType)                            \
  void vulkan_debug_name_##_funcName(vulkan_debug *debug, _handleType handle, const char *format,  \
                                     ...) {                                                        \
    va_list args;                                                                                  \
    va_start(args, format);                                                                        \
    name_object(debug, _objectType, (uint64_t)handle, format, args);                               \
    va_end(args);                                                                                  \
  }

VULKAN_DEBUG_NAME_FUNC_DEF(command_pool, VkCommandPool, VK_OBJECT_TYPE_COMMAND_POOL)
VULKAN_DEBUG_NAME_FUNC_DEF(command_buffer, VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER)
VULKAN_DEBUG_NAME_FUNC_DEF(framebuffer, VkFramebuffer, VK_OBJECT_TYPE_FRAMEBUFFER)
VULKAN_DEBUG_NAME_FUNC_DEF(semaphore, VkSemaphore, VK_OBJECT_TYPE_SEMAPHORE)
VULKAN_DEBUG_NAME_FUNC_DEF(fence, VkFence, VK_OBJECT_TYPE_FENCE)
VULKAN_DEBUG_NAME_FUNC_DEF(device_memory, VkDeviceMemory, VK_OBJECT_TYPE_DEVICE_MEMORY)
VULKAN_DEBUG_NAME_FUNC_DEF(buffer, VkBuffer, VK_OBJECT_TYPE_BUFFER)
VULKAN_DEBUG_NAME_FUNC_DEF(image, VkImage, VK_OBJECT_TYPE_IMAGE)
VULKAN_DEBUG_NAME_FUNC_DEF(image_view, VkImageView, VK_OBJECT_TYPE_IMAGE_VIEW)
VULKAN_DEBUG_NAME_FUNC_DEF(shader_module, VkShaderModule, VK_OBJECT_TYPE_SHADER_MODULE)
VULKAN_DEBUG_NAME_FUNC_DEF(descriptor_pool, VkDescriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL)
VULKAN_DEBUG_NAME_FUNC_DEF(descriptor_set_layout, VkDescriptorSetLayout,
                           VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT)
VULKAN_DEBUG_NAME_FUNC_DEF(descriptor_set, VkDescriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET)
VULKAN_DEBUG_NAME_FUNC_DEF(pipeline_layout, VkPipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT)

#undef VULKAN_DEBUG_NAME_FUNC_DEF

VkBool32 VKAPI_CALL vulkan_debug_callback_for_instance(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  log_level logLevel;
  UT_string *msg;
  utstring_new(msg);

  debug_callback_log_prepare(messageSeverity, messageType, pCallbackData, &logLevel, msg);
  log_pretty(logLevel, __func__, __FILE__, __LINE__, "vulkan_debug_callback_instance: %s",
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

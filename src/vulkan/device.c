#include "device.h"
#include "../codegen/device.c"

#include "../codegen/vulkan_core.c"

const char *validationLayers[VALIDATION_LAYERS_SIZE] = {"VK_LAYER_KHRONOS_validation"};

const char *deviceExtensions[DEVICE_EXTENSIONS_SIZE] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const VkAllocationCallbacks *vka = NULL;

VkResult create_debug_utils_messenger_ext(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                                                                "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
                                                                 "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL) {
    func(instance, debugMessenger, pAllocator);
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
               VkDebugUtilsMessageTypeFlagsEXT messageType,
               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  log_error("Validation layer: %s", pCallbackData->pMessage);
  return VK_FALSE;
}

bool vulkan_queue_families_complete(vulkan_queue_families *queueFamilies) {
  return queueFamilies->graphicsFamily < UINT32_MAX && queueFamilies->presentFamily < UINT32_MAX;
}

void vulkan_swap_chain_info_init(vulkan_swap_chain_info *vksInfo) {
  vksInfo->formats = vec_VkSurfaceFormatKHR_init();
  vksInfo->presentModes = vec_VkPresentModeKHR_init();
}

void vulkan_swap_chain_info_deinit(vulkan_swap_chain_info *vksInfo) {
  vec_VkSurfaceFormatKHR_free(&vksInfo->formats);
  vec_VkPresentModeKHR_free(&vksInfo->presentModes);
}

void vulkan_device_init(vulkan_device *vkd, data_config *config) {
  vulkan_swap_chain_info_init(&vkd->swapChainInfo);
  create_window(vkd, config);
  create_instance(vkd, config);
  setup_debug_messenger(vkd);
  create_surface(vkd);
  pick_physical_device(vkd);
  create_logical_device(vkd);
  create_one_shot_command_pool(vkd);
}

void vulkan_device_deinit(vulkan_device *vkd) {
  log_info("vulkan_device_deinit()");
  vulkan_swap_chain_info_deinit(&vkd->swapChainInfo);

  vkDestroyCommandPool(vkd->device, vkd->oneShotCommandPool, vka);

  vkDestroyDevice(vkd->device, vka);

  if (validation_layers_enabled()) {
    destroy_debug_utils_messenger_ext(vkd->instance, vkd->debugMessenger, NULL);
  }

  vkDestroySurfaceKHR(vkd->instance, vkd->surface, vka);

  vkDestroyInstance(vkd->instance, vka);

  glfwDestroyWindow(vkd->window);
  glfwTerminate();
}

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
  vulkan_device *vkd = glfwGetWindowUserPointer(window);
  vkd->framebufferResized = true;
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {}

void glfw_mouse_callback(GLFWwindow *window, double xPos, double yPos) {}

void create_window(vulkan_device *vkd, data_config *config) {
  log_info("create_window");
  verify(glfwInit() == GLFW_TRUE);
  verify(glfwVulkanSupported() == GLFW_TRUE);
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  vkd->window = glfwCreateWindow(config->windowWidth, config->windowHeight,
                                 str_c_str(&config->windowTitle), NULL, NULL);
  glfwSetWindowUserPointer(vkd->window, vkd);
  glfwSetFramebufferSizeCallback(vkd->window, glfw_framebuffer_resize_callback);
  glfwSetKeyCallback(vkd->window, glfw_key_callback);
  glfwSetCursorPosCallback(vkd->window, glfw_mouse_callback);
  vkd->framebufferResized = false;
  // glfwSetInputMode(vkd->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool validation_layers_enabled() {
#if defined(DEBUG)
  return true;
#else
  return false;
#endif
}

bool check_validation_layer_support(vulkan_device *vkd) {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);
  VkLayerProperties *availableLayers =
      (VkLayerProperties *)malloc(layerCount * sizeof(VkLayerProperties));
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  for (size_t i = 0; i < VALIDATION_LAYERS_SIZE; i++) {
    const char *layerName = validationLayers[i];
    bool layerFound = false;
    for (size_t j = 0; j < layerCount; j++) {
      VkLayerProperties layerProperties = availableLayers[j];
      log_debug("validation layer: %s", layerProperties.layerName);
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      free(availableLayers);
      return false;
    }
  }
  free(availableLayers);
  return true;
}

void create_instance(vulkan_device *vkd, data_config *config) {
  if (validation_layers_enabled() && !check_validation_layer_support(vkd)) {
    panic("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo = {0};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = str_c_str(&config->windowTitle);
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = str_c_str(&config->windowTitle);
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwInstanceExtensionCount = 0;
  const char **glfwInstanceExtensions;
  glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwInstanceExtensionCount);
  uint32_t instanceExtensionCount = glfwInstanceExtensionCount;
  if (validation_layers_enabled()) {
    instanceExtensionCount += 1;
  }
  const char **instanceExtensions =
      (const char **)malloc(instanceExtensionCount * sizeof(const char *));
  for (uint32_t i = 0; i < glfwInstanceExtensionCount; i++) {
    instanceExtensions[i] = glfwInstanceExtensions[i];
  }
  if (validation_layers_enabled()) {
    instanceExtensions[instanceExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  }
  createInfo.enabledExtensionCount = instanceExtensionCount;
  createInfo.ppEnabledExtensionNames = instanceExtensions;

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
  if (validation_layers_enabled()) {
    createInfo.enabledLayerCount = VALIDATION_LAYERS_SIZE;
    createInfo.ppEnabledLayerNames = validationLayers;

    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debug_callback;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;
  }

  verify(vkCreateInstance(&createInfo, vka, &vkd->instance) == VK_SUCCESS);

  free(instanceExtensions);
}

void setup_debug_messenger(vulkan_device *vkd) {
  if (!validation_layers_enabled()) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
  debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debugCreateInfo.pfnUserCallback = debug_callback;

  verify(create_debug_utils_messenger_ext(vkd->instance, &debugCreateInfo, vka,
                                          &vkd->debugMessenger) == VK_SUCCESS);
}

void create_surface(vulkan_device *vkd) {
  verify(glfwCreateWindowSurface(vkd->instance, vkd->window, vka, &vkd->surface) == VK_SUCCESS);
}

void query_swap_chain_support(vulkan_device *vkd, VkPhysicalDevice physicalDevice) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkd->surface,
                                            &vkd->swapChainInfo.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkd->surface, &formatCount, NULL);

  if (formatCount != 0) {
    VkSurfaceFormatKHR undefinedSurfaceFormat = {0};
    vec_VkSurfaceFormatKHR_resize(&vkd->swapChainInfo.formats, formatCount, undefinedSurfaceFormat);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkd->surface, &formatCount,
                                         vec_VkSurfaceFormatKHR_data(&vkd->swapChainInfo.formats));
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkd->surface, &presentModeCount, NULL);

  if (presentModeCount != 0) {
    VkPresentModeKHR undefinedPresentMode = {0};
    vec_VkPresentModeKHR_resize(&vkd->swapChainInfo.presentModes, presentModeCount,
                                undefinedPresentMode);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice, vkd->surface, &presentModeCount,
        vec_VkPresentModeKHR_data(&vkd->swapChainInfo.presentModes));
  }
}

bool check_device_extension_support(vulkan_device *vkd, VkPhysicalDevice physicalDevice) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);

  VkExtensionProperties *availableExtensions =
      (VkExtensionProperties *)malloc(extensionCount * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);

  bool missingExtension = false;
  for (size_t i = 0; i < DEVICE_EXTENSIONS_SIZE; i++) {
    const char *extensionName = deviceExtensions[i];
    missingExtension = true;
    for (size_t j = 0; j < extensionCount; j++) {
      VkExtensionProperties extension = availableExtensions[j];
      if (strcmp(extensionName, extension.extensionName) == 0) {
        missingExtension = false;
        break;
      }
    }
    if (missingExtension) {
      log_error("unsupported extension: %s", extensionName);
      break;
    }
  }

  free(availableExtensions);
  return !missingExtension;
}

bool is_physical_device_suitable(vulkan_device *vkd, VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  log_info("deviceId = %X", deviceProperties.deviceID);

  uint32_t versionMajor = VK_API_VERSION_MAJOR(deviceProperties.apiVersion);
  uint32_t versionMinor = VK_API_VERSION_MINOR(deviceProperties.apiVersion);
  uint32_t versionPatch = VK_API_VERSION_PATCH(deviceProperties.apiVersion);
  log_info("apiVersion = %d.%d.%d", versionMajor, versionMinor, versionPatch);
  bool goodVulkanVersion = versionMajor >= 1 && versionMinor >= 2;

  vulkan_queue_families queueFamilies = find_queue_families(vkd, physicalDevice);
  bool queueFamiliesComplete = vulkan_queue_families_complete(&queueFamilies);
  log_info("found queue familes = %d", queueFamiliesComplete);

  bool extensionsSupported = check_device_extension_support(vkd, physicalDevice);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    query_swap_chain_support(vkd, physicalDevice);
    swapChainAdequate =
        vkd->swapChainInfo.formats.size > 0 && vkd->swapChainInfo.presentModes.size > 0;
  }
  log_info("swapChainAdequate = %d", swapChainAdequate);

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);
  log_info("samplerAnisotropy = %d", supportedFeatures.samplerAnisotropy);
  log_info("shaderUniformBufferArrayDynamicIndexing = %d",
           supportedFeatures.shaderUniformBufferArrayDynamicIndexing);
  log_info("shaderSampledImageArrayDynamicIndexing = %d",
           supportedFeatures.shaderSampledImageArrayDynamicIndexing);
  bool featuresSupported = supportedFeatures.samplerAnisotropy &&
                           supportedFeatures.shaderUniformBufferArrayDynamicIndexing &&
                           supportedFeatures.shaderSampledImageArrayDynamicIndexing;

  return queueFamiliesComplete && extensionsSupported && swapChainAdequate && goodVulkanVersion &&
         featuresSupported;
}

void pick_physical_device(vulkan_device *vkd) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vkd->instance, &deviceCount, NULL);
  verify(deviceCount > 0);
  VkPhysicalDevice *devices = (VkPhysicalDevice *)malloc(deviceCount * sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices(vkd->instance, &deviceCount, devices);

  for (size_t i = 0; i < deviceCount; i++) {
    VkPhysicalDevice physicalDevice = devices[i];
    if (is_physical_device_suitable(vkd, physicalDevice)) {
      vkd->physicalDevice = physicalDevice;
      break;
    }
  }
  free(devices);
  verify(vkd->physicalDevice != VK_NULL_HANDLE);

  vkd->limits = find_limits(vkd, vkd->physicalDevice);
}

vulkan_queue_families find_queue_families(vulkan_device *vkd, VkPhysicalDevice physicalDevice) {
  vulkan_queue_families result = {0};
  result.graphicsFamily = UINT32_MAX;
  result.presentFamily = UINT32_MAX;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

  VkQueueFamilyProperties *queueFamilies =
      (VkQueueFamilyProperties *)malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

  int queueFamilyIndex = 0;
  for (size_t i = 0; i < queueFamilyCount; i++) {
    VkQueueFamilyProperties queueFamily = queueFamilies[i];
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      result.graphicsFamily = queueFamilyIndex;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vkd->surface, &presentSupport);
    if (presentSupport) {
      result.presentFamily = queueFamilyIndex;
    }
    if (vulkan_queue_families_complete(&result)) {
      break;
    }
    queueFamilyIndex++;
  }
  free(queueFamilies);
  return result;
}

vulkan_limits find_limits(vulkan_device *vkd, VkPhysicalDevice physicalDevice) {
  vulkan_limits limits;
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  limits.maxImageDimension2D = deviceProperties.limits.maxImageDimension2D;
  limits.maxUniformBufferRange = deviceProperties.limits.maxUniformBufferRange;
  limits.maxStorageBufferRange = deviceProperties.limits.maxStorageBufferRange;
  limits.maxPushConstantsSize = deviceProperties.limits.maxPushConstantsSize;
  limits.maxPerStageDescriptorUniformBuffers =
      deviceProperties.limits.maxPerStageDescriptorUniformBuffers;
  limits.maxPerStageDescriptorStorageBuffers =
      deviceProperties.limits.maxPerStageDescriptorStorageBuffers;
  limits.maxPerStageDescriptorSampledImages =
      deviceProperties.limits.maxPerStageDescriptorSampledImages;
  limits.maxPerStageResources = deviceProperties.limits.maxPerStageResources;
  limits.maxBoundDescriptorSets = deviceProperties.limits.maxBoundDescriptorSets;
  limits.maxVertexInputAttributes = deviceProperties.limits.maxVertexInputAttributes;
  limits.maxVertexOutputComponents = deviceProperties.limits.maxVertexOutputComponents;
  return limits;
}

void create_logical_device(vulkan_device *vkd) {
  vulkan_queue_families queueFamilies = find_queue_families(vkd, vkd->physicalDevice);

  VkDeviceQueueCreateInfo *queueCreateInfos;
  uint32_t numQueues;
  float queuePriority = 1.0F;
  if (queueFamilies.graphicsFamily != queueFamilies.presentFamily) {
    numQueues = 2;
    queueCreateInfos =
        (VkDeviceQueueCreateInfo *)malloc(numQueues * sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].pNext = NULL;
    queueCreateInfos[0].flags = 0;
    queueCreateInfos[0].queueFamilyIndex = queueFamilies.graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;
    queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[1].pNext = NULL;
    queueCreateInfos[1].flags = 0;
    queueCreateInfos[1].queueFamilyIndex = queueFamilies.presentFamily;
    queueCreateInfos[1].queueCount = 1;
    queueCreateInfos[1].pQueuePriorities = &queuePriority;
  } else {
    numQueues = 1;
    queueCreateInfos =
        (VkDeviceQueueCreateInfo *)malloc(numQueues * sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].pNext = NULL;
    queueCreateInfos[0].flags = 0;
    queueCreateInfos[0].queueFamilyIndex = queueFamilies.graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;
  }

  VkPhysicalDeviceFeatures deviceFeatures = {0};
  deviceFeatures.samplerAnisotropy = VK_TRUE;
  deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
  deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;

  VkDeviceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = numQueues;
  createInfo.pQueueCreateInfos = queueCreateInfos;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = DEVICE_EXTENSIONS_SIZE;
  createInfo.ppEnabledExtensionNames = deviceExtensions;

  if (validation_layers_enabled()) {
    createInfo.enabledLayerCount = VALIDATION_LAYERS_SIZE;
    createInfo.ppEnabledLayerNames = validationLayers;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  verify(vkCreateDevice(vkd->physicalDevice, &createInfo, vka, &vkd->device) == VK_SUCCESS);
  free(queueCreateInfos);

  vkGetDeviceQueue(vkd->device, queueFamilies.graphicsFamily, 0, &vkd->graphicsQueue);
  vkGetDeviceQueue(vkd->device, queueFamilies.presentFamily, 0, &vkd->presentQueue);
}

void create_one_shot_command_pool(vulkan_device *vkd) {
  vulkan_queue_families queueFamilies = find_queue_families(vkd, vkd->physicalDevice);

  // TODO: Use transfer queue - but vkCmdBlitImage is graphics queue only?
  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily;

  verify(vkCreateCommandPool(vkd->device, &poolInfo, vka, &vkd->oneShotCommandPool) == VK_SUCCESS);
}
#include "device.h"

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height);
void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow *window, double x, double y);

void create_window(device *vkd, data_config *config, data_asset_db *assetDb);

bool validation_layers_enabled();
bool check_validation_layer_support(device *vkd);
void create_instance(device *vkd, data_config *config, data_asset_db *assetDb);
void create_debug_utils(device *vkd);
void create_surface(device *vkd);

void query_swap_chain_support(device *vkd, VkPhysicalDevice physicalDevice);
bool check_device_extension_support(device *vkd, VkPhysicalDevice physicalDevice);
bool physical_device_suitable(device *vkd, VkPhysicalDevice physicalDevice, size_t *rank);
void pick_physical_device(device *vkd);

queue_families find_queue_families(device *vkd, VkPhysicalDevice physicalDevice);
limits find_limits(device *vkd, VkPhysicalDevice physicalDevice);
void create_logical_device(device *vkd);

void create_one_shot_command_pool(device *vkd);

const char *validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

VkValidationFeatureEnableEXT enabledValidationFeatures[] = {
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
    // VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
};

const char *instanceExtensions[] = {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};

const char *deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
#if defined(DEBUG)
    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, // NOTE: Required by debugPrintf.
#endif
};

const VkAllocationCallbacks *vka = NULL;

bool queue_families_complete(queue_families *queueFamilies) {
  return queueFamilies->graphicsFamily < UINT32_MAX && queueFamilies->presentFamily < UINT32_MAX;
}

void swap_chain_support_init(swap_chain_support *support) {
  utarray_new(support->formats, &ut_vk_surface_format_icd);
  utarray_new(support->presentModes, &ut_vk_present_mode_icd);
}

void swap_chain_support_deinit(swap_chain_support *support) {
  utarray_free(support->formats);
  utarray_free(support->presentModes);
}

device *device_create(data_config *config, data_asset_db *assetDb) {
  device *vkd = core_alloc(sizeof(device));
  swap_chain_support_init(&vkd->swapChainSupport);
  vkd->swapChainInfo.imageFormat = VK_FORMAT_UNDEFINED;
  vkd->swapChainInfo.extent = (VkExtent2D){0};
  create_window(vkd, config, assetDb);
  create_instance(vkd, config, assetDb);
  create_debug_utils(vkd);
  create_surface(vkd);
  pick_physical_device(vkd);
  create_logical_device(vkd);
  create_one_shot_command_pool(vkd);
  vkd->input = input_default();
  return vkd;
}

void device_destroy(device *vkd) {
  log_info("device_destroy()");
  swap_chain_support_deinit(&vkd->swapChainSupport);

  vkDestroyCommandPool(vkd->device, vkd->oneShotCommandPool, vka);

  vkDestroyDevice(vkd->device, vka);

  debug_destroy(vkd->debug);

  vkDestroySurfaceKHR(vkd->instance, vkd->surface, vka);

  vkDestroyInstance(vkd->instance, vka);

  glfwDestroyWindow(vkd->window);
  glfwTerminate();

  core_free(vkd);
}

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
  device *vkd = glfwGetWindowUserPointer(window);
  vkd->framebufferResized = true;
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (!(action == GLFW_PRESS || action == GLFW_RELEASE)) {
    return; // Ignore GLFW_REPEAT.
  }
  device *vkd = glfwGetWindowUserPointer(window);
#define input_key(_name, _glfwKey, ...)                                                            \
  if (key == _glfwKey) {                                                                           \
    vkd->input.keyboard.press._name = action == GLFW_PRESS;                                        \
    vkd->input.keyboard.release._name = action == GLFW_RELEASE;                                    \
  }
  VULKAN_INPUT_KEYS(input_key, )
#undef input_key
}

void glfw_mouse_callback(GLFWwindow *window, double x, double y) {
  device *vkd = glfwGetWindowUserPointer(window);
  static bool skip = true; // True if first callback after leaving/recreating window.
  if (skip) {
    vkd->input.mouse.lastX = x;
    vkd->input.mouse.lastY = y;
    skip = false;
  }
  vkd->input.mouse.x = x;
  vkd->input.mouse.y = y;
}

void create_window(device *vkd, data_config *config, data_asset_db *assetDb) {
  log_info("create_window");
  verify(glfwInit() == GLFW_TRUE);
  verify(glfwVulkanSupported() == GLFW_TRUE);
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  verify(config->asset.graphicsWindowWidth > 0 && config->asset.graphicsWindowHeight > 0);
  vkd->window =
      glfwCreateWindow(config->asset.graphicsWindowWidth, config->asset.graphicsWindowHeight,
                       utstring_body(config->asset.graphicsWindowTitle), NULL, NULL);
  glfwSetWindowUserPointer(vkd->window, vkd);
  glfwSetFramebufferSizeCallback(vkd->window, glfw_framebuffer_resize_callback);
  glfwSetKeyCallback(vkd->window, glfw_key_callback);
  glfwSetCursorPosCallback(vkd->window, glfw_mouse_callback);
  glfwSetInputMode(vkd->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  vkd->framebufferResized = false;
}

bool validation_layers_enabled() {
#if defined(DEBUG)
  return true;
#else
  return false;
#endif
}

bool check_validation_layer_support(device *vkd) {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);
  VkLayerProperties *availableLayers =
      (VkLayerProperties *)malloc(layerCount * sizeof(VkLayerProperties));
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  for (size_t i = 0; i < array_size(validationLayers); i++) {
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
      log_debug("validation layer %s not available!", layerName);
      free(availableLayers);
      return false;
    }
  }
  free(availableLayers);
  return true;
}

void create_instance(device *vkd, data_config *config, data_asset_db *assetDb) {
  if (validation_layers_enabled() && !check_validation_layer_support(vkd)) {
    panic("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo = {0};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = utstring_body(config->asset.graphicsWindowTitle);
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = utstring_body(config->asset.graphicsWindowTitle);
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwInstanceExtensionCount = 0;
  const char **glfwInstanceExtensions;
  glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwInstanceExtensionCount);
  uint32_t instanceExtensionCount = glfwInstanceExtensionCount + array_size(instanceExtensions);
  if (validation_layers_enabled()) {
    instanceExtensionCount += 1;
  }
  const char *mergedInstanceExtensions[instanceExtensionCount];
  for (uint32_t i = 0; i < glfwInstanceExtensionCount; i++) {
    mergedInstanceExtensions[i] = glfwInstanceExtensions[i];
  }
  for (uint32_t i = 0; i < array_size(instanceExtensions); i++) {
    mergedInstanceExtensions[glfwInstanceExtensionCount + i] = instanceExtensions[i];
  }
  if (validation_layers_enabled()) {
    mergedInstanceExtensions[instanceExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  }
  createInfo.enabledExtensionCount = instanceExtensionCount;
  createInfo.ppEnabledExtensionNames = mergedInstanceExtensions;

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
  VkValidationFeaturesEXT validationFeatures = {0};
  if (validation_layers_enabled()) {
    createInfo.enabledLayerCount = array_size(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;
    debugCreateInfo = debug_messenger_create_info(debug_callback_for_instance);
    validationFeatures = (VkValidationFeaturesEXT){
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
        .enabledValidationFeatureCount = array_size(enabledValidationFeatures),
        .pEnabledValidationFeatures = enabledValidationFeatures,
    };
    debugCreateInfo.pNext = &validationFeatures;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;
  }

  verify(vkCreateInstance(&createInfo, vka, &vkd->instance) == VK_SUCCESS);
}

void create_debug_utils(device *vkd) {
  vkd->device = VK_NULL_HANDLE;
  vkd->debug = debug_create(validation_layers_enabled(), &vkd->device, vkd->instance, vka);
}

void create_surface(device *vkd) {
  verify(glfwCreateWindowSurface(vkd->instance, vkd->window, vka, &vkd->surface) == VK_SUCCESS);
}

void query_swap_chain_support(device *vkd, VkPhysicalDevice physicalDevice) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vkd->surface,
                                            &vkd->swapChainSupport.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkd->surface, &formatCount, NULL);

  if (formatCount != 0) {
    utarray_resize(vkd->swapChainSupport.formats, formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vkd->surface, &formatCount,
                                         utarray_front(vkd->swapChainSupport.formats));
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkd->surface, &presentModeCount, NULL);

  if (presentModeCount != 0) {
    utarray_resize(vkd->swapChainSupport.presentModes, presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vkd->surface, &presentModeCount,
                                              utarray_front(vkd->swapChainSupport.presentModes));
  }
}

void device_query_swap_chain_support(device *vkd) {
  assert(vkd->physicalDevice != VK_NULL_HANDLE);
  query_swap_chain_support(vkd, vkd->physicalDevice);
}

bool check_device_extension_support(device *vkd, VkPhysicalDevice physicalDevice) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);

  VkExtensionProperties *availableExtensions =
      (VkExtensionProperties *)malloc(extensionCount * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);

  bool missingExtension = false;
  for (size_t i = 0; i < array_size(deviceExtensions); i++) {
    const char *extensionName = deviceExtensions[i];
    missingExtension = true;
    for (size_t j = 0; j < extensionCount; j++) {
      VkExtensionProperties extension = availableExtensions[j];
      log_debug("ext: %s", extension.extensionName);
      if (strcmp(extensionName, extension.extensionName) == 0) {
        log_debug("ext found: %s", extension.extensionName);
        missingExtension = false;
        break;
      }
    }
    if (missingExtension) {
      break;
    }
  }

  free(availableExtensions);
  return !missingExtension;
}

bool physical_device_suitable(device *vkd, VkPhysicalDevice physicalDevice, size_t *rank) {
  log_info("physical device info:");

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  log_info("deviceId = %X", deviceProperties.deviceID);
  log_info("deviceName = %s", deviceProperties.deviceName);

  // Favor discrete GPUs.
  switch (deviceProperties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    *rank += 100;
    break;
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    *rank += 10;
    break;
  default:
    *rank += 1;
  }
  uint32_t versionMajor = VK_API_VERSION_MAJOR(deviceProperties.apiVersion);
  uint32_t versionMinor = VK_API_VERSION_MINOR(deviceProperties.apiVersion);
  uint32_t versionPatch = VK_API_VERSION_PATCH(deviceProperties.apiVersion);
  log_info("apiVersion = %d.%d.%d", versionMajor, versionMinor, versionPatch);
  bool goodVulkanVersion = versionMajor >= 1 && versionMinor >= 2;

  vkd->queueFamilies = find_queue_families(vkd, physicalDevice);
  bool queueFamiliesComplete = queue_families_complete(&vkd->queueFamilies);
  log_info("found queue familes = %d", queueFamiliesComplete);

  bool extensionsSupported = check_device_extension_support(vkd, physicalDevice);
  log_info("extensionsSupported = %d", extensionsSupported);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    query_swap_chain_support(vkd, physicalDevice);
    swapChainAdequate = utarray_len(vkd->swapChainSupport.formats) > 0 &&
                        utarray_len(vkd->swapChainSupport.presentModes) > 0;
  }
  log_info("swapChainAdequate = %d", swapChainAdequate);

  VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan11Features features11 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  VkPhysicalDeviceVulkan12Features features12 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  VkPhysicalDeviceRobustness2FeaturesEXT featuresRobustness2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT};
  VkPhysicalDeviceDynamicRenderingFeaturesKHR featuresDynamicRendering = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR};
  physicalDeviceFeatures2.pNext = &features11;
  features11.pNext = &features12;
  features12.pNext = &featuresRobustness2;
  featuresRobustness2.pNext = &featuresDynamicRendering;
  vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
  VkPhysicalDeviceFeatures features10 = physicalDeviceFeatures2.features;
  log_info("samplerAnisotropy = %d", features10.samplerAnisotropy);
  log_info("shaderUniformBufferArrayDynamicIndexing = %d",
           features10.shaderUniformBufferArrayDynamicIndexing);
  log_info("shaderSampledImageArrayDynamicIndexing = %d",
           features10.shaderSampledImageArrayDynamicIndexing);
  log_info("nullDescriptor = %d", featuresRobustness2.nullDescriptor);
  log_info("dynamicRendering = %d", featuresDynamicRendering.dynamicRendering);
  log_info("descriptorIndexing = %d", features12.descriptorIndexing);
  log_info("shaderSampledImageArrayNonUniformIndexing = %d",
           features12.shaderSampledImageArrayNonUniformIndexing);
  log_info("descriptorBindingVariableDescriptorCount = %d",
           features12.descriptorBindingVariableDescriptorCount);
  log_info("descriptorBindingPartiallyBound = %d", features12.descriptorBindingPartiallyBound);
  log_info("runtimeDescriptorArray = %d", features12.runtimeDescriptorArray);
  // Scalar block layout allows us to use std430 memory layout for GLSL buffers, which corresponds
  // more directly to C struct alignment rules.
  log_info("scalarBlockLayout = %d", features12.scalarBlockLayout);
  log_info("multiDrawIndirect = %d", features10.multiDrawIndirect);
  log_info("drawIndirectFirstInstance = %d", features10.drawIndirectFirstInstance);
  bool featuresSupported =
      features10.samplerAnisotropy && features10.shaderUniformBufferArrayDynamicIndexing &&
      features10.shaderSampledImageArrayDynamicIndexing && featuresRobustness2.nullDescriptor &&
      features12.descriptorIndexing && features12.descriptorBindingVariableDescriptorCount &&
      features12.descriptorBindingPartiallyBound && features12.runtimeDescriptorArray &&
      features12.scalarBlockLayout && featuresDynamicRendering.dynamicRendering &&
      features10.multiDrawIndirect && features10.drawIndirectFirstInstance;

  return queueFamiliesComplete && extensionsSupported && swapChainAdequate && goodVulkanVersion &&
         featuresSupported;
}

typedef struct physical_device_info {
  VkPhysicalDevice physicalDevice;
  size_t rank;
} physical_device_info;

static int physical_device_info_compare(const void *s1, const void *s2) {
  physical_device_info *a1 = (physical_device_info *)s1;
  physical_device_info *a2 = (physical_device_info *)s2;
  return (int)a2->rank - (int)a1->rank;
}

static const UT_icd ut_physical_device_info_icd = {sizeof(physical_device_info), NULL, NULL, NULL};

void pick_physical_device(device *vkd) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vkd->instance, &deviceCount, NULL);
  verify(deviceCount > 0);
  VkPhysicalDevice *devices = (VkPhysicalDevice *)malloc(deviceCount * sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices(vkd->instance, &deviceCount, devices);

  vkd->physicalDevice = VK_NULL_HANDLE;
  UT_array *infos; // physical_device_info
  utarray_new(infos, &ut_physical_device_info_icd);
  for (size_t i = 0; i < deviceCount; i++) {
    VkPhysicalDevice physicalDevice = devices[i];
    size_t rank = 0;
    if (physical_device_suitable(vkd, physicalDevice, &rank)) {
      physical_device_info info = {.physicalDevice = physicalDevice, .rank = rank};
      utarray_push_back(infos, &info);
    }
  }
  if (utarray_len(infos) == 0) {
    panic("failed to find suitable GPU");
  }
  qsort(infos, utarray_len(infos), sizeof(physical_device_info), physical_device_info_compare);
  vkd->physicalDevice = ((physical_device_info *)utarray_front(infos))->physicalDevice;
  utarray_free(infos);
  free(devices);
  verify(vkd->physicalDevice != VK_NULL_HANDLE);

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(vkd->physicalDevice, &deviceProperties);
  log_info("device found: deviceId = %X, deviceName = %s", deviceProperties.deviceID,
           deviceProperties.deviceName);

  vkd->queueFamilies = find_queue_families(vkd, vkd->physicalDevice);
  device_query_swap_chain_support(vkd);
  vkd->limits = find_limits(vkd, vkd->physicalDevice);
}

queue_families find_queue_families(device *vkd, VkPhysicalDevice physicalDevice) {
  queue_families result = {0};
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
    if (queue_families_complete(&result)) {
      break;
    }
    queueFamilyIndex++;
  }
  free(queueFamilies);
  return result;
}

limits find_limits(device *vkd, VkPhysicalDevice physicalDevice) {
  VkPhysicalDeviceDescriptorIndexingProperties descriptorIndexingProperties = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES};
  VkPhysicalDeviceProperties2 deviceProperties2 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = &descriptorIndexingProperties};
  vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties2);
  VkPhysicalDeviceProperties deviceProperties = deviceProperties2.properties;

  limits limits;

  limits.maxImageDimension2D = deviceProperties.limits.maxImageDimension2D;
  limits.maxUniformBufferRange = deviceProperties.limits.maxUniformBufferRange;
  limits.maxStorageBufferRange = deviceProperties.limits.maxStorageBufferRange;
  limits.minUniformBufferOffsetAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;

  limits.maxPerStageDescriptorUniformBuffers =
      deviceProperties.limits.maxPerStageDescriptorUniformBuffers;
  limits.maxPerStageBindlessDescriptorUniformBuffers =
      descriptorIndexingProperties.maxDescriptorSetUpdateAfterBindUniformBuffers;
  limits.maxPerStageDescriptorStorageBuffers =
      deviceProperties.limits.maxPerStageDescriptorStorageBuffers;
  limits.maxPerStageBindlessDescriptorStorageBuffers =
      descriptorIndexingProperties.maxDescriptorSetUpdateAfterBindStorageBuffers;
  limits.maxPerStageDescriptorSampledImages =
      deviceProperties.limits.maxPerStageDescriptorSampledImages;
  limits.maxPerStageBindlessDescriptorSampledImages =
      descriptorIndexingProperties.maxDescriptorSetUpdateAfterBindSampledImages;

  limits.maxPerStageResources = deviceProperties.limits.maxPerStageResources;
  limits.maxPerStageBindlessResources =
      descriptorIndexingProperties.maxPerStageUpdateAfterBindResources;

  limits.maxBoundDescriptorSets = deviceProperties.limits.maxBoundDescriptorSets;
  limits.maxVertexInputAttributes = deviceProperties.limits.maxVertexInputAttributes;
  limits.maxVertexOutputComponents = deviceProperties.limits.maxVertexOutputComponents;
  limits.maxVertexInputBindings = deviceProperties.limits.maxVertexInputBindings;
  limits.maxVertexInputBindingStride = deviceProperties.limits.maxVertexInputBindingStride;

  limits.maxDrawIndirectCount = deviceProperties.limits.maxDrawIndirectCount;
  limits.maxDrawIndirectCommands =
      MIN(limits.maxDrawIndirectCount, MAX_INDIRECT_DRAW_COMMAND_COUNT);

  return limits;
}

void create_logical_device(device *vkd) {
  VkDeviceQueueCreateInfo *queueCreateInfos;
  uint32_t numQueues;
  float queuePriority = 1.0f;
  if (vkd->queueFamilies.graphicsFamily != vkd->queueFamilies.presentFamily) {
    numQueues = 2;
    queueCreateInfos =
        (VkDeviceQueueCreateInfo *)malloc(numQueues * sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].pNext = NULL;
    queueCreateInfos[0].flags = 0;
    queueCreateInfos[0].queueFamilyIndex = vkd->queueFamilies.graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;
    queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[1].pNext = NULL;
    queueCreateInfos[1].flags = 0;
    queueCreateInfos[1].queueFamilyIndex = vkd->queueFamilies.presentFamily;
    queueCreateInfos[1].queueCount = 1;
    queueCreateInfos[1].pQueuePriorities = &queuePriority;
  } else {
    numQueues = 1;
    queueCreateInfos =
        (VkDeviceQueueCreateInfo *)malloc(numQueues * sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].pNext = NULL;
    queueCreateInfos[0].flags = 0;
    queueCreateInfos[0].queueFamilyIndex = vkd->queueFamilies.graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;
  }

  VkPhysicalDeviceFeatures deviceFeatures = {
      .samplerAnisotropy = VK_TRUE,
      .shaderUniformBufferArrayDynamicIndexing = VK_TRUE,
      .shaderSampledImageArrayDynamicIndexing = VK_TRUE,
      .multiDrawIndirect = VK_TRUE,
      .drawIndirectFirstInstance = VK_TRUE,
  };
  VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
      .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
      .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
      .descriptorBindingVariableDescriptorCount = VK_TRUE,
      .descriptorBindingPartiallyBound = VK_TRUE,
      .runtimeDescriptorArray = VK_TRUE,
  };
  VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayoutFeatures = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES,
      .scalarBlockLayout = VK_TRUE,
  };
  VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
      .dynamicRendering = true,
  };
  VkPhysicalDeviceFeatures2 deviceFeatures2 = {.sType =
                                                   VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                                               .features = deviceFeatures,
                                               .pNext = &descriptorIndexingFeatures};
  descriptorIndexingFeatures.pNext = &scalarBlockLayoutFeatures;
  scalarBlockLayoutFeatures.pNext = &dynamicRenderingFeatures;

  VkDeviceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = numQueues;
  createInfo.pQueueCreateInfos = queueCreateInfos;
  createInfo.pNext = &deviceFeatures2; // createInfo.pEnabledFeatures = NULL;
  createInfo.enabledExtensionCount = array_size(deviceExtensions);
  createInfo.ppEnabledExtensionNames = deviceExtensions;

  if (validation_layers_enabled()) {
    createInfo.enabledLayerCount = array_size(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  verify(vkCreateDevice(vkd->physicalDevice, &createInfo, vka, &vkd->device) == VK_SUCCESS);
  free(queueCreateInfos);

  vkGetDeviceQueue(vkd->device, vkd->queueFamilies.graphicsFamily, 0, &vkd->graphicsQueue);
  vkGetDeviceQueue(vkd->device, vkd->queueFamilies.presentFamily, 0, &vkd->presentQueue);

  vkd->cmdBeginRendering =
      (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(vkd->instance, "vkCmdBeginRenderingKHR");
  vkd->cmdEndRendering =
      (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(vkd->instance, "vkCmdEndRenderingKHR");
  verify(vkd->cmdBeginRendering);
  verify(vkd->cmdEndRendering);
}

void create_one_shot_command_pool(device *vkd) {
  vkd->oneShotCommandPool =
      create_command_pool(vkd, vkd->queueFamilies.graphicsFamily, 0, "one-shot");
  // TODO: Also use transfer queue - but vkCmdBlitImage is graphics queue only?
}

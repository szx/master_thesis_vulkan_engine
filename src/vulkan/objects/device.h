/* Vulkan API device helper functions and structures */
#pragma once

#include "../../core/core.h"
#include "../../data/data.h"
#include "../constants.h"
#include "../debug.h"
#include "device_functions.h"
#include "input.h"

extern const VkAllocationCallbacks *vka;

typedef struct queue_families {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
} queue_families;

bool queue_families_complete(queue_families *queueFamilies);

static const UT_icd ut_vk_surface_format_icd = {sizeof(VkSurfaceFormatKHR), NULL, NULL, NULL};
static const UT_icd ut_vk_present_mode_icd = {sizeof(VkPresentModeKHR), NULL, NULL, NULL};

typedef struct swap_chain_info {
  VkSurfaceCapabilitiesKHR capabilities;
  UT_array *formats;      /// VkSurfaceFormatKHR
  UT_array *presentModes; /// VkPresentModeKHR
} swap_chain_info;

void swap_chain_info_init(swap_chain_info *vksInfo);
void swap_chain_info_deinit(swap_chain_info *vksInfo);

typedef struct limits {
  /* Vulkan API limits */
  uint32_t maxImageDimension2D;
  uint32_t maxUniformBufferRange;
  uint32_t maxStorageBufferRange;
  uint32_t minUniformBufferOffsetAlignment;

  uint32_t maxPerStageDescriptorUniformBuffers;
  uint32_t maxPerStageBindlessDescriptorUniformBuffers;
  uint32_t maxPerStageDescriptorStorageBuffers;
  uint32_t maxPerStageBindlessDescriptorStorageBuffers;
  uint32_t maxPerStageDescriptorSampledImages;
  uint32_t maxPerStageBindlessDescriptorSampledImages;

  uint32_t maxPerStageResources;
  uint32_t maxPerStageBindlessResources;

  uint32_t maxBoundDescriptorSets;
  uint32_t maxVertexInputAttributes;
  uint32_t maxVertexOutputComponents;
  uint32_t maxVertexInputBindings;
  uint32_t maxVertexInputBindingStride;

  size_t maxDrawIndirectCount;
  size_t maxDrawIndirectCommands;
} limits;

typedef struct device {
  GLFWwindow *window;              ///< GLFW window
  VkInstance instance;             ///< Vulkan instance.
  VkSurfaceKHR surface;            ///< Vulkan window surface.
  debug *debug;                    ///< Vulkan debug utils.
  VkPhysicalDevice physicalDevice; ///< Physical device.

  VkDevice device;       ///< Vulkan logical device.
  VkQueue graphicsQueue; ///< Graphical queue handle.
  VkQueue presentQueue;  ///< Present queue handle.
  PFN_vkCmdBeginRenderingKHR cmdBeginRendering;
  PFN_vkCmdEndRenderingKHR cmdEndRendering;

  swap_chain_info swapChainInfo; ///< Swap chain support details.
  VkFormat swapChainImageFormat; ///< Chosen swap chain image format.
  VkExtent2D swapChainExtent;    ///< Chosen swap chain image format.
  limits limits;                 ///< Physical device limits.
  bool framebufferResized;       ///< True if GLFW framebuffer resize callback was triggered.
  VkCommandPool
      oneShotCommandPool; ///< Command pool used for one-shot copy and image transition commands.
  input input;            ///< GLFW keyboard and mouse input.

} device;

device *device_create(data_config *config, data_asset_db *assetDb);
void device_destroy(device *vkd);

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

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

/// Information about swap chain support.
typedef struct swap_chain_support {
  VkSurfaceCapabilitiesKHR capabilities;
  UT_array *formats;      /// VkSurfaceFormatKHR
  UT_array *presentModes; /// VkPresentModeKHR
} swap_chain_support;

void swap_chain_support_init(swap_chain_support *support);
void swap_chain_support_deinit(swap_chain_support *support);

/// Information about created swap chain.
typedef struct swap_chain_info {
  VkFormat imageFormat; ///< Chosen swap chain image format.
  VkExtent2D extent;    ///< Chosen swap chain image extent.
} swap_chain_info;

typedef struct limits {
  /* Vulkan API limits */
  float maxSamplerAnisotropy;

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

  VkDevice device; ///< Vulkan logical device.

  queue_families queueFamilies;
  VkQueue graphicsQueue; ///< Graphical queue handle.
  VkQueue presentQueue;  ///< Present queue handle.

  PFN_vkCmdBeginRenderingKHR cmdBeginRendering;
  PFN_vkCmdEndRenderingKHR cmdEndRendering;

  swap_chain_support swapChainSupport; ///< Swap chain support details.
  swap_chain_info swapChainInfo;       /// Created swap chain info.

  limits limits;           ///< Physical device limits.
  bool framebufferResized; ///< True if GLFW framebuffer resize callback was triggered.

  VkCommandPool
      oneShotCommandPool; ///< Command pool used for one-shot copy and image transition commands.

  input input; ///< GLFW keyboard and mouse input.

} device;

device *device_create(data_config *config, data_asset_db *assetDb);
void device_destroy(device *vkd);

void device_query_swap_chain_support(device *vkd);

/* Vulkan debug messengers. */

#pragma once

#include "../core/platform.h"

/// Vulkan debug utils.
/// Manages Vulkan debug messengers and their additional data.
typedef struct vulkan_debug {
  bool enabled;                            /// True if we use debug utils.
  VkInstance instance;                     /// Vulkan instance handle passed from vulkan_device.
  const VkAllocationCallbacks *vka;        /// Vulkan allocator passed from vulkan_device.
  VkDebugUtilsMessengerEXT debugMessenger; /// Vulkan debug messenger (VK_EXT_debug_utils).
} vulkan_debug;

vulkan_debug *vulkan_debug_create(bool enabled, VkInstance instance,
                                  const VkAllocationCallbacks *vka);
void vulkan_debug_destroy(vulkan_debug *debug);

/// Short-lived debug callback for vkCreateInstance and vkCreateInstance.
/// Logs output of validation layers.
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback_for_instance(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

/// Returns create info for debug messenger with user callback.
VkDebugUtilsMessengerCreateInfoEXT
vulkan_debug_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback);

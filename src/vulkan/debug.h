/* Vulkan debug messengers. */

#pragma once

#include "../core/platform.h"

/// Hash table for Vulkan object names and their associated additional data.
typedef struct vulkan_debug_name_data {
  const char *name;
  UT_hash_handle hh;
} vulkan_debug_name_data;

/// Vulkan debug utils.
/// Manages Vulkan debug messengers.
typedef struct vulkan_debug {
  bool enabled;                            /// True if we use debug utils.
  VkDevice *pDevice;                       /// Pointer to vulkan device handle.
  VkInstance instance;                     /// Vulkan instance handle.
  const VkAllocationCallbacks *vka;        /// Vulkan allocator.
  VkDebugUtilsMessengerEXT debugMessenger; /// Vulkan debug messenger (VK_EXT_debug_utils).
  /** \addtogroup Functions pointers to EXT functions.
   *  @{
   */
  PFN_vkCmdBeginDebugUtilsLabelEXT cmdBeginDebugUtilsLabelEXT;
  PFN_vkCmdEndDebugUtilsLabelEXT cmdEndDebugUtilsLabelEXT;
  PFN_vkCmdInsertDebugUtilsLabelEXT cmdInsertDebugUtilsLabelEXT;
  PFN_vkSetDebugUtilsObjectNameEXT setDebugUtilsObjectNameEXT;
  /** @}*/
  vulkan_debug_name_data *names; /// Hash table with Vulkan object names and their data.
} vulkan_debug;

vulkan_debug *vulkan_debug_create(bool enabled, VkDevice *device, VkInstance instance,
                                  const VkAllocationCallbacks *vka);
void vulkan_debug_destroy(vulkan_debug *debug);

#define VULKAN_DEBUG_NAME_FUNC_DECL(_funcName, _handleType)                                        \
  void vulkan_debug_name_##_funcName(vulkan_debug *debug, _handleType handle, const char *format,  \
                                     ...);

VULKAN_DEBUG_NAME_FUNC_DECL(command_pool, VkCommandPool)
VULKAN_DEBUG_NAME_FUNC_DECL(command_buffer, VkCommandBuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(framebuffer, VkFramebuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(semaphore, VkSemaphore)
VULKAN_DEBUG_NAME_FUNC_DECL(fence, VkFence)
VULKAN_DEBUG_NAME_FUNC_DECL(device_memory, VkDeviceMemory)
VULKAN_DEBUG_NAME_FUNC_DECL(buffer, VkBuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(image, VkImage)
VULKAN_DEBUG_NAME_FUNC_DECL(image_view, VkImageView)
VULKAN_DEBUG_NAME_FUNC_DECL(shader_module, VkShaderModule)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_pool, VkDescriptorPool)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_set_layout, VkDescriptorSetLayout)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_set, VkDescriptorSet)
VULKAN_DEBUG_NAME_FUNC_DECL(pipeline_layout, VkPipelineLayout)

#undef VULKAN_DEBUG_NAME_FUNC_DECL

/// Short-lived debug callback for vkCreateInstance and vkCreateInstance.
/// Logs output of validation layers.
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback_for_instance(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

/// Returns create info for debug messenger with user callback.
VkDebugUtilsMessengerCreateInfoEXT
vulkan_debug_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback);

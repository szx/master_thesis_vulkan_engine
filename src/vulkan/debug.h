/* Vulkan debug messengers. */

#pragma once

#include "../core/platform.h"

/// Hash table for Vulkan object names and their associated additional data.
typedef struct debug_name_data {
  const char *name;
  UT_hash_handle hh;
} debug_name_data;

/// Vulkan debug utils.
/// Manages Vulkan debug messengers.
typedef struct debug {
  bool enabled;                            ///< True if we use debug utils.
  void *vkd;                               ///< Pointer to device object.
  VkDebugUtilsMessengerEXT debugMessenger; ///< Vulkan debug messenger (VK_EXT_debug_utils).
  debug_name_data *names;                  ///< Hash table with Vulkan object names and their data.
} debug;

debug *debug_create(bool enabled, void *vkd);
void debug_destroy(debug *debug);

#define VULKAN_DEBUG_NAME_FUNC_DECL(_funcName, _handleType)                                        \
  void debug_name_##_funcName(debug *debug, _handleType handle, const char *format, ...);

VULKAN_DEBUG_NAME_FUNC_DECL(command_pool, VkCommandPool)
VULKAN_DEBUG_NAME_FUNC_DECL(command_buffer, VkCommandBuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(framebuffer, VkFramebuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(semaphore, VkSemaphore)
VULKAN_DEBUG_NAME_FUNC_DECL(fence, VkFence)
VULKAN_DEBUG_NAME_FUNC_DECL(device_memory, VkDeviceMemory)
VULKAN_DEBUG_NAME_FUNC_DECL(buffer, VkBuffer)
VULKAN_DEBUG_NAME_FUNC_DECL(image, VkImage)
VULKAN_DEBUG_NAME_FUNC_DECL(image_view, VkImageView)
VULKAN_DEBUG_NAME_FUNC_DECL(sampler, VkSampler)
VULKAN_DEBUG_NAME_FUNC_DECL(shader_module, VkShaderModule)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_pool, VkDescriptorPool)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_set_layout, VkDescriptorSetLayout)
VULKAN_DEBUG_NAME_FUNC_DECL(descriptor_set, VkDescriptorSet)
VULKAN_DEBUG_NAME_FUNC_DECL(pipeline_layout, VkPipelineLayout)
VULKAN_DEBUG_NAME_FUNC_DECL(render_pass, VkRenderPass)
VULKAN_DEBUG_NAME_FUNC_DECL(graphics_pipeline, VkPipeline)

#undef VULKAN_DEBUG_NAME_FUNC_DECL

/// Short-lived debug callback for vkCreateInstance and vkCreateInstance.
/// Logs output of validation layers.
VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback_for_instance(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

/// Returns create info for debug messenger with user callback.
VkDebugUtilsMessengerCreateInfoEXT
debug_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback);

#define DEBUG_NAME_FORMAT_START()                                                                  \
  va_list args;                                                                                    \
  va_start(args, debugFormat);                                                                     \
  char *debugName;                                                                                 \
  UT_string *s;                                                                                    \
  utstring_new(s);                                                                                 \
  utstring_printf_va(s, debugFormat, args);                                                        \
  va_end(args);                                                                                    \
  debugName = core_strdup(utstring_body(s));                                                       \
  utstring_free(s);

#define DEBUG_NAME_FORMAT_END() free(debugName);

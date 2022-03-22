/* Vulkan API swap chain */
#pragma once

#include "../common.h"
#include "device.h"

typedef struct vulkan_swap_chain {
  vulkan_device *vkd;        /// vulkan_device pointer.
  VkSwapchainKHR swapChain;  /// Vulkan swap chain.
  UT_array *swapChainImages; /// Swap chain images - a collection of two or more
                             /// VKImages used to present rendering results to screen.
                             /// Swap chain images are obtained by
                             /// vkGetSwapchainImagesKHR() - they are neither created
                             /// nor owned by application! Currently swap chain images
                             /// are used by framebuffers and render passes as resolve
                             /// attachments.

  VkFormat swapChainImageFormat;         /// Format of swap chain image.
  VkPresentModeKHR swapChainPresentMode; /// Presentation mode.
  VkExtent2D swapChainExtent;            /// Size of swap chain image (in pixels).

  UT_array *swapChainImageViews; ///< VkImageView array.
} vulkan_swap_chain;

vulkan_swap_chain *vulkan_swap_chain_create(vulkan_device *vkd);
void vulkan_swap_chain_destroy(vulkan_swap_chain *vks);
float vulkan_swap_chain_get_aspect_ratio(vulkan_swap_chain *vks);

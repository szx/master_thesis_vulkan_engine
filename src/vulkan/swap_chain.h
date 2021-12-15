/* Vulkan API swap chain */

#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "../data/config.h"
#include "device.h"
#include "functions.h"

#define MAX_FRAMES_IN_FLIGHT 2
#define MIN_DELTA_TIME (1.0 / 60.0)

#define P
#define T VkImage
#include "vec.h" // vec_VkImage
#define P
#define T VkImageView
#include "vec.h" // vec_VkImageView

typedef struct vulkan_swap_chain {
  vulkan_device *vkd;       /// vulkan_device pointer.
  VkSwapchainKHR swapChain; /// Vulkan swap chain.
  vec_VkImage
      swapChainImages; /// Swap chain images - a collection of two or more
                       /// images  used to present rendering results to screen.
                       /// Swap chain images are obtained by
                       /// vkGetSwapchainImagesKHR() - they are neither created
                       /// nor owned by application! Currently swap chain images
                       /// are used by framebuffers and render passes as resolve
                       /// attachments.

  VkFormat swapChainImageFormat;         /// Format of swap chain image.
  VkPresentModeKHR swapChainPresentMode; /// Presentation mode.
  VkExtent2D swapChainExtent; /// Size of swap chain image (in pixels).

  vec_VkImageView
      swapChainImageViews; /// Image views describing swap chain images.
                           /// Necessary because Vulkan API tends to refer to
                           /// resources via views.
} vulkan_swap_chain;

void vulkan_swap_chain_init(vulkan_swap_chain *vks, vulkan_device *vkd);
void vulkan_swap_chain_deinit(vulkan_swap_chain *vks);

VkSurfaceFormatKHR
choose_swap_surface_format(vec_VkSurfaceFormatKHR *availableFormats);
VkPresentModeKHR
choose_swap_present_mode(vec_VkPresentModeKHR *availablePresentModes);
VkExtent2D choose_swap_extent(vulkan_device *vkd);

void create_swap_chain(vulkan_swap_chain *vks);
void get_swap_chain_images(vulkan_swap_chain *vks);
void create_swap_chain_image_views(vulkan_swap_chain *vks);

#include "../codegen/swap_chain.h"

#endif /* !VULKAN_SWAP_CHAIN_H */

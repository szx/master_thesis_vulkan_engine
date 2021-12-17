#include "swap_chain.h"
#include "../codegen/swap_chain.c"

void vulkan_swap_chain_init(vulkan_swap_chain *vks, vulkan_device *vkd) {
  vks->vkd = vkd;
  vks->swapChain = VK_NULL_HANDLE;
  vks->swapChainImageViews = vec_VkImageView_init();
  vks->swapChainImages = vec_VkImage_init();
  create_swap_chain(vks);
  get_swap_chain_images(vks);
  create_swap_chain_image_views(vks);
}

void vulkan_swap_chain_deinit(vulkan_swap_chain *vks) {
  for (size_t i = 0; i < vks->swapChainImageViews.size; i++) {
    vkDestroyImageView(vks->vkd->device, vks->swapChainImageViews.value[i], vka);
  }
  vec_VkImageView_free(&vks->swapChainImageViews);
  vec_VkImage_free(&vks->swapChainImages);

  vkDestroySwapchainKHR(vks->vkd->device, vks->swapChain, vka);
  vks->swapChain = VK_NULL_HANDLE;
}

VkSurfaceFormatKHR choose_swap_surface_format(UT_array *availableFormats) {
  for (size_t i = 0; i < utarray_len(availableFormats); i++) {
    VkSurfaceFormatKHR availableFormat = *(VkSurfaceFormatKHR *)utarray_eltptr(availableFormats, i);
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }
  return *(VkSurfaceFormatKHR *)utarray_front(availableFormats);
}

VkPresentModeKHR choose_swap_present_mode(UT_array *availablePresentModes) {
  for (size_t i = 0; i < utarray_len(availablePresentModes); i++) {
    VkPresentModeKHR availablePresentMode =
        *(VkPresentModeKHR *)utarray_eltptr(availablePresentModes, i);
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }
  return *(VkPresentModeKHR *)utarray_front(availablePresentModes);
}

VkExtent2D choose_swap_extent(vulkan_device *vkd) {
  VkSurfaceCapabilitiesKHR capabilities = vkd->swapChainInfo.capabilities;
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }
  int width;
  int height;
  glfwGetFramebufferSize(vkd->window, &width, &height);
  VkExtent2D actualExtent = {(uint32_t)width, (uint32_t)height};
  actualExtent.width = MAX(capabilities.minImageExtent.width,
                           MIN(capabilities.maxImageExtent.width, actualExtent.width));
  actualExtent.height = MAX(capabilities.minImageExtent.height,
                            MIN(capabilities.maxImageExtent.height, actualExtent.height));
  return actualExtent;
}

void create_swap_chain(vulkan_swap_chain *vks) {
  assert(vks->swapChain == VK_NULL_HANDLE);
  query_swap_chain_support(vks->vkd, vks->vkd->physicalDevice);
  vulkan_swap_chain_info *swapChainInfo = &vks->vkd->swapChainInfo;
  VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainInfo->formats);
  VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainInfo->presentModes);
  VkExtent2D extent = choose_swap_extent(vks->vkd);

  uint32_t imageCount = swapChainInfo->capabilities.minImageCount + 1;
  if (swapChainInfo->capabilities.maxImageCount > 0 &&
      imageCount > swapChainInfo->capabilities.maxImageCount) {
    imageCount = swapChainInfo->capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = vks->vkd->surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  vulkan_queue_families queueFamilies = find_queue_families(vks->vkd, vks->vkd->physicalDevice);
  uint32_t queueFamiliesIndices[] = {queueFamilies.graphicsFamily, queueFamilies.presentFamily};

  if (queueFamilies.graphicsFamily != queueFamilies.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamiliesIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainInfo->capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  verify(vkCreateSwapchainKHR(vks->vkd->device, &createInfo, vka, &vks->swapChain) == VK_SUCCESS);

  vks->swapChainImageFormat = surfaceFormat.format;
  vks->swapChainExtent = extent;
}

void get_swap_chain_images(vulkan_swap_chain *vks) {
  assert(vks->swapChain != VK_NULL_HANDLE);
  assert(vks->swapChainImages.size == 0);

  uint32_t imageCount = 0;
  vkGetSwapchainImagesKHR(vks->vkd->device, vks->swapChain, &imageCount, NULL);
  vec_VkImage_resize(&vks->swapChainImages, imageCount, VK_NULL_HANDLE);
  vkGetSwapchainImagesKHR(vks->vkd->device, vks->swapChain, &imageCount,
                          vec_VkImage_data(&vks->swapChainImages));
}

void create_swap_chain_image_views(vulkan_swap_chain *vks) {
  assert(vks->swapChainImages.size > 0);
  assert(vks->swapChainImageViews.size == 0);
  assert(vks->swapChainImageFormat != VK_FORMAT_UNDEFINED);

  vec_VkImageView_resize(&vks->swapChainImageViews, vks->swapChainImages.size, VK_NULL_HANDLE);

  for (size_t i = 0; i < vks->swapChainImageViews.size; i++) {
    vks->swapChainImageViews.value[i] =
        create_image_view(vks->vkd, vks->swapChainImages.value[i], VK_IMAGE_VIEW_TYPE_2D,
                          vks->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
  }
}

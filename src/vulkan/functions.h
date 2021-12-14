/* Vulkan API and helper functions and structures */

#ifndef VULKAN_FUNCTIONS_H
#define VULKAN_FUNCTIONS_H

#include "../core/platform.h"
#include "../data/config.h"

#include "../codegen/vulkan_core.h"

#define VALIDATION_LAYERS_SIZE 1
extern const char *validationLayers[VALIDATION_LAYERS_SIZE];
#define DEVICE_EXTENSIONS_SIZE 1
extern const char *deviceExtensions[DEVICE_EXTENSIONS_SIZE];

extern const VkAllocationCallbacks *vka;

typedef struct vulkan_queue_families {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
} vulkan_queue_families;

bool vulkan_queue_families_complete(vulkan_queue_families *queueFamilies);

#define P
#define T VkSurfaceFormatKHR
#include "vec.h" // vec_VkSurfaceFormatKHR
#define P
#define T VkPresentModeKHR
#include "vec.h" // vec_VkPresentModeKHR

typedef struct vulkan_swap_chain_info {
  VkSurfaceCapabilitiesKHR capabilities;
  vec_VkSurfaceFormatKHR formats;
  vec_VkPresentModeKHR presentModes;
} vulkan_swap_chain_info;

void vulkan_swap_chain_info_init(vulkan_swap_chain_info *vksInfo);
void vulkan_swap_chain_info_deinit(vulkan_swap_chain_info *vksInfo);

typedef struct vulkan_limits {
  /* Vulkan API limits */
  uint32_t maxImageDimension2D;
  uint32_t maxUniformBufferRange;
  uint32_t maxStorageBufferRange;
  uint32_t maxPushConstantsSize;
  uint32_t maxPerStageDescriptorUniformBuffers;
  uint32_t maxPerStageDescriptorStorageBuffers;
  uint32_t maxPerStageDescriptorSampledImages;
  uint32_t maxPerStageResources;
  uint32_t maxBoundDescriptorSets;
  uint32_t maxVertexInputAttributes;
  uint32_t maxVertexOutputComponents;
  /* render context limits */
  // TODO: Rethink.
  uint32_t maxSceneNodes; /// Max scene nodes per scene.
  uint32_t maxTextures;   /// Max number of textures per scene.
  uint32_t maxLights;     /// Max number of lights per scene.
} vulkan_limits;

typedef struct vulkan_device {
  GLFWwindow *window;                      /// GLFW window
  VkInstance instance;                     /// Vulkan instance.
  VkDebugUtilsMessengerEXT debugMessenger; /// Vulkan debug messenger (VK_EXT_debug_utils).
  VkSurfaceKHR surface;                    /// Vulkan window surface.
  VkPhysicalDevice physicalDevice;         /// Physical device.
  VkDevice device;                         /// Vulkan logical device.
  VkQueue graphicsQueue;                   /// Graphical queue handle.
  VkQueue presentQueue;                 /// Present queue handle.
  vulkan_swap_chain_info swapChainInfo; /// Swap chain support details.
  vulkan_limits limits;                 /// Physical device limits.
  bool framebufferResized; /// True if GLFW framebuffer resize callback was
                           /// triggered.
  VkCommandPool oneShotCommandPool; /// Command pool used for one-shot copy and
                                    /// image transition commands.
  struct {
    bool w, a, s, d;
    bool q;
    bool num1, num2, num3, num4;
    bool esc;
  } input; /// GLFW keyboard input.

  struct {
    float sensitivity;
    double xPos, yPos;
    double xLastPos, yLastPos;
  } mouse; /// GLFW mouse input.
} vulkan_device;

void vulkan_device_init(vulkan_device *vkd, data_config *config);
void vulkan_device_deinit(vulkan_device *vkd);

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width,
                                      int height);
void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods);
void glfw_mouse_callback(GLFWwindow *window, double xPos, double yPos);

void create_window(vulkan_device *vkd, data_config *config);

bool validation_layers_enabled();
bool check_validation_layer_support(vulkan_device *vkd);
void create_instance(vulkan_device *vkd, data_config *config);
void setup_debug_messenger(vulkan_device *vkd);

void create_surface(vulkan_device *vkd);

void query_swap_chain_support(vulkan_device *vkd,
                              VkPhysicalDevice physicalDevice);
bool check_device_extension_support(vulkan_device *vkd,
                                    VkPhysicalDevice physicalDevice);
bool is_physical_device_suitable(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
void pick_physical_device(vulkan_device *vkd);

vulkan_queue_families find_queue_families(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
vulkan_limits find_limits(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
void create_logical_device(vulkan_device *vkd);

void create_one_shot_command_pool(vulkan_device *vkd);

/* Vulkan helper functions */
// TODO: Implementations all Vulkan helper functions.
uint32_t find_memory_type(vulkan_device *vkd, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
VkFormat find_depth_format(vulkan_device *vkd);
void create_image(vulkan_device *vkd, uint32_t width, uint32_t height, uint32_t mipLevels,
                  uint32_t arrayLayers, VkSampleCountFlagBits numSamples, VkFormat format,
                  VkImageTiling tiling, VkImageCreateFlags flags, VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkImage image, VkDeviceMemory imageMemory);
VkImageView create_image_view(vulkan_device *vkd, VkImage image, VkImageViewType type,
                              VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              uint32_t arrayLayers);
void create_buffer(vulkan_device *vkd, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer *buffer,
                   VkDeviceMemory *bufferMemory);

VkCommandBuffer begin_single_time_commands(vulkan_device *vkd);
void end_single_time_commands(vulkan_device *vkd,
                              VkCommandBuffer commandBuffer);
void copy_buffer(vulkan_device *vkd, VkBuffer srcBuffer, VkBuffer dstBuffer,
                 VkDeviceSize size);
void copy_buffer_to_image(vulkan_device *vkd, VkBuffer buffer, VkImage image,
                          uint32_t width, uint32_t height,
                          uint32_t baseArrayLayer);
void generate_mipmaps(vulkan_device *vkd, VkImage image, VkFormat imageFormat,
                      int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
void transition_image_layout(vulkan_device *vkd, VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels, uint32_t arrayLayers);

VkShaderModule create_shader_module(vulkan_device *vkd, const uint32_t *code, size_t size);
// VkDescriptorSetLayout create_descriptor_set_layout(VkDescriptorType
// descriptorType, uint32_t descriptorCount, VkShaderStageFlags stageFlags);
// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::vector<VkBuffer> uniformBuffers, VkDeviceSize bufferSize,
// VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);
// VkDescriptorSet createDescriptorSet(VkDescriptorType descriptorType,
// std::set<std::shared_ptr<Texture>> textures, VkDescriptorSetLayout
// descriptorSetLayout, VkDescriptorPool descriptorPool);
// std::tuple<VkPipelineLayout, VkPipeline> createGraphicsPipeline(PipelineType
// type, RenderPassName name, const ShaderConstants &shaderConstants,
// VkRenderPass renderPass, size_t numOutputColorAttachments, uint32_t width,
// uint32_t height, std::vector<VkDescriptorSetLayout> setLayouts);

#include "../codegen/functions.h"

#endif /* !VULKAN_FUNCTIONS_H */

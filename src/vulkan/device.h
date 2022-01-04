/* Vulkan API device helper functions and structures */

#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "../core/platform.h"
#include "../data/config.h"

#include "../codegen/vulkan_core.h"

#define VALIDATION_LAYERS_SIZE 1
extern const char *validationLayers[VALIDATION_LAYERS_SIZE];
#define INSTANCE_EXTENSIONS_SIZE 1
extern const char *instanceExtensions[INSTANCE_EXTENSIONS_SIZE];
#define DEVICE_EXTENSIONS_SIZE 2
extern const char *deviceExtensions[DEVICE_EXTENSIONS_SIZE];

extern const VkAllocationCallbacks *vka;

typedef struct vulkan_queue_families {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
} vulkan_queue_families;

bool vulkan_queue_families_complete(vulkan_queue_families *queueFamilies);

static const UT_icd ut_vk_surface_format_icd = {sizeof(VkSurfaceFormatKHR), NULL, NULL, NULL};
static const UT_icd ut_vk_present_mode_icd = {sizeof(VkPresentModeKHR), NULL, NULL, NULL};

typedef struct vulkan_swap_chain_info {
  VkSurfaceCapabilitiesKHR capabilities;
  UT_array *formats;      /// VkSurfaceFormatKHR
  UT_array *presentModes; /// VkPresentModeKHR
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
  uint32_t maxVertexInputBindings;
  uint32_t maxVertexInputBindingStride;
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
  VkQueue presentQueue;                    /// Present queue handle.
  vulkan_swap_chain_info swapChainInfo;    /// Swap chain support details.
  vulkan_limits limits;                    /// Physical device limits.
  bool framebufferResized;                 /// True if GLFW framebuffer resize callback was
                                           /// triggered.
  VkCommandPool oneShotCommandPool;        /// Command pool used for one-shot copy and
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

vulkan_device *vulkan_device_create(data_config *config);
void vulkan_device_destroy(vulkan_device *vkd);

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height);
void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_mouse_callback(GLFWwindow *window, double xPos, double yPos);

void create_window(vulkan_device *vkd, data_config *config);

bool validation_layers_enabled();
bool check_validation_layer_support(vulkan_device *vkd);
void create_instance(vulkan_device *vkd, data_config *config);
void setup_debug_messenger(vulkan_device *vkd);

void create_surface(vulkan_device *vkd);

void query_swap_chain_support(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
bool check_device_extension_support(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
bool is_physical_device_suitable(vulkan_device *vkd, VkPhysicalDevice physicalDevice, size_t *rank);
void pick_physical_device(vulkan_device *vkd);

vulkan_queue_families find_queue_families(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
vulkan_limits find_limits(vulkan_device *vkd, VkPhysicalDevice physicalDevice);
void create_logical_device(vulkan_device *vkd);

void create_one_shot_command_pool(vulkan_device *vkd);

#include "../codegen/device.h"

#endif /* !VULKAN_DEVICE_H */

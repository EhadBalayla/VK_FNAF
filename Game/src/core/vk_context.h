#ifndef VK_CONTEXT
#define VK_CONTEXT

#include <vulkan/vulkan.h>

extern int MAX_FRAMES_IN_FLIGHT;

typedef struct {
    uint32_t graphicsFamily; int hasGraphics;
    uint32_t presentFamily; int hasPresent;
} QueueFamilyIndicies;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats; uint32_t formatCount;
    VkPresentModeKHR* presentModes; uint32_t presentModesCount;
} SwapChainSupportDetails;

typedef struct {
    void* pWindow;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    VkQueue graphicsQueue;
    VkQueue PresentQueue;
} vk_context;

void VKContext_InitGPU(vk_context* pContext);
void VKContext_TerminateGPU(vk_context* pContext);


QueueFamilyIndicies FindQueueFamilies(VkPhysicalDevice device, vk_context* pContext);
SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, vk_context* pContext);

#endif
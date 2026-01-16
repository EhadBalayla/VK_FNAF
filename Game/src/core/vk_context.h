#ifndef VK_CONTEXT
#define VK_CONTEXT

#include <vulkan/vulkan.h>

typedef struct {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
} vk_context;

void VKContext_InitGPU(vk_context* pContext);
void VKContext_TerminateGPU(vk_context* pContext);


#endif
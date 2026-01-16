#ifndef VK_SWAPCHAIN
#define VK_SWAPCHAIN

#include <vulkan/vulkan.h>

extern uint32_t imageIndex;

typedef struct {
    void* pWindow;
    void* pContext; //a reference to the context too
    int MaxFramesInFlight;

    VkSwapchainKHR swapchain;
    VkImage* swapchainImages; uint32_t swapchainImagesCount;
    VkImageView* swapchainImageViews; //uses the same amount as the swapchainImages
    VkFramebuffer* swapchainFramebuffers; //same as the image views
    VkSemaphore* imageAvailableSemaphores; //again, the same thing
    VkRenderPass swapchainRenderPass;

    VkFormat swapchainImageFormat;
    VkExtent2D swapchainImageExtent;


} vk_swapchain;

void VKSwapchain_Create(vk_swapchain* pSwapchain);
void VKSwapchain_Delete(vk_swapchain* pSwapchain);

void VKSwapchain_Recreate(vk_swapchain* pSwapchain);

#endif
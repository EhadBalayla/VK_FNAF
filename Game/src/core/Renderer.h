#ifndef RENDERER
#define RENDERER

#include <vulkan/vulkan.h>

extern int currentFrame;

typedef struct {
    //Vulkan setup handles (already created externally, will just be passing references)
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    VkQueue graphicsQueue;
    VkQueue PresentQueue;
    int MaxFramesInFlight;

    //for the arrays, the sizes are the max frames in flight
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    VkSemaphore* renderingFinishedSemaphores;
    VkFence* inFlightFences;
} Renderer;

void Renderer_Init(Renderer* pRenderer);
void Renderer_Terminate(Renderer* pRenderer);

void Renderer_wait(Renderer* pRenderer);

#endif
#ifndef RENDERER
#define RENDERER

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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
    VkDescriptorPool descriptorPool;
    VmaAllocator allocator;
    VkSampler sampler;

    //descriptors, also arrays too use the max frames in flight
    VkDescriptorSetLayout singleTexLayout; //a set layout for a single texture 
    VkDescriptorSet* officeTextureSets; //a single descriptor set hardcoded to be used for the office
    
} Renderer;

void Renderer_Init(Renderer* pRenderer);
void Renderer_Terminate(Renderer* pRenderer);

//we need separate functions since we wanna make those descriptor sets after we load in all the textures
void Renderer_CreateSets(Renderer* pRenderer);

void Renderer_wait(Renderer* pRenderer);

#endif
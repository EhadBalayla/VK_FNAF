#ifndef RENDERER
#define RENDERER

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

extern int currentFrame;

#define MAX_RENDERS 2

#define OFFICE 0
#define OFFICE_FAN 1

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
    VkRenderPass offscreenPass;
    VkImage* colorBuffer;
    VkImageView* colorBufferView;
    VmaAllocation* colorBufferAllocation;
    VkFramebuffer* offscreenFramebuffers;

    //descriptors, also arrays too use the max frames in flight
    VkDescriptorSetLayout singleTexLayout; //a set layout for a single texture 
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet* fullscreenSets; //will be plopping the offscreen buffer into this for using in the swapchain
    VkDescriptorSet* textureSets[MAX_RENDERS];
} Renderer;

void Renderer_Init(Renderer* pRenderer);
void Renderer_Terminate(Renderer* pRenderer);

//we need separate functions since we wanna make those descriptor sets after we load in all the textures
void Renderer_CreateSets(Renderer* pRenderer);

//for the offscreen drawing
void Renderer_StartDraw(Renderer* pRenderer);
void Renderer_EndDraw(Renderer* pRenderer);

void Renderer_wait(Renderer* pRenderer);

void Renderer_RecreateOffscreenFramebuffer(Renderer* pRenderer);

#endif
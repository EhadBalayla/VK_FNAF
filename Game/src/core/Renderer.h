#ifndef RENDERER
#define RENDERER

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

extern int currentFrame;

#define MAX_RENDERS 14

#define OFFICE 0
#define OFFICE_FAN 1
#define MONITOR_HOVER 2
#define MONITOR_FLIP 3
#define CAMERA_STATIC 4
#define CAMERA_UNSELECTED 5
#define CAMERA_SELECTED 6
#define CAMERA_STATIC_ALPHA 7
#define TCAM1 8
#define TCAM2 9
#define TCAM3 10
#define TCAM4 11
#define LEFT_DOOR 12
#define RIGHT_DOOR 13

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
    VkDescriptorSet* fontSets; //will be used for the font texture, because the font isn't a texture in the files its loaded dynamically from a TTF file
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
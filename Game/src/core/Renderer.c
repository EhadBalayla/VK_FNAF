#include "Renderer.h"

#include <stdio.h>
#include <stdlib.h>

#include "Game.h"

int currentFrame = 0;

//creation functions declarations
void CreateCommandPool(Renderer* pRenderer);
void CreateCommandBuffers(Renderer* pRenderer);
void CreateDescriptorPool(Renderer* pRenderer);
void CreateAllocator(Renderer* pRenderer);
void CreateSampler(Renderer* pRenderer);

void Renderer_Init(Renderer* pRenderer) {
    CreateCommandPool(pRenderer);
    CreateCommandBuffers(pRenderer);
    CreateDescriptorPool(pRenderer);
    CreateAllocator(pRenderer);
    CreateSampler(pRenderer);
}
void Renderer_Terminate(Renderer* pRenderer) {
    //destroy stuff that are arrays with the max frames in flight stuff
    free(pRenderer->officeTextureSets);

    vkDestroyDescriptorSetLayout(pRenderer->device, pRenderer->singleTexLayout, NULL);

    vkDestroySampler(pRenderer->device, pRenderer->sampler, NULL);

    vkFreeCommandBuffers(pRenderer->device, pRenderer->commandPool, pRenderer->MaxFramesInFlight, pRenderer->commandBuffers);
    
    vmaDestroyAllocator(pRenderer->allocator);

    vkDestroyDescriptorPool(pRenderer->device, pRenderer->descriptorPool, NULL);

    free(pRenderer->commandBuffers);

    vkDestroyCommandPool(pRenderer->device, pRenderer->commandPool, NULL);
}

void Renderer_CreateSets(Renderer* pRenderer) {
    VkDescriptorSetLayoutBinding TexBinding = {0};
    TexBinding.binding = 0;
    TexBinding.descriptorCount = 1;
    TexBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TexBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    TexBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pBindings = &TexBinding;
    layoutInfo.bindingCount = 1;

    if(vkCreateDescriptorSetLayout(pRenderer->device, &layoutInfo, NULL, &pRenderer->singleTexLayout) != VK_SUCCESS) {
        fprintf(stderr, "failed to create the descriptor set layout for a single texture");
        exit(EXIT_FAILURE);
    }

    VkDescriptorSetLayout setLayouts[] = {pRenderer->singleTexLayout, pRenderer->singleTexLayout, pRenderer->singleTexLayout};
    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pRenderer->descriptorPool;
    allocInfo.descriptorSetCount = pRenderer->MaxFramesInFlight;
    allocInfo.pSetLayouts = setLayouts;

    pRenderer->officeTextureSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * pRenderer->MaxFramesInFlight);
    if(vkAllocateDescriptorSets(pRenderer->device, &allocInfo, pRenderer->officeTextureSets) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate all single texture descriptor sets");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        VkDescriptorImageInfo imgInfo = {0};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgInfo.imageView = GGame->officeTexture.imageView;
        imgInfo.sampler = pRenderer->sampler;

        VkWriteDescriptorSet setWrite = {0};
        setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        setWrite.descriptorCount = 1;
        setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        setWrite.dstBinding = 0;
        setWrite.pImageInfo = &imgInfo;
        setWrite.dstArrayElement = 0;
        setWrite.dstSet = pRenderer->officeTextureSets[i];

        vkUpdateDescriptorSets(pRenderer->device, 1, &setWrite, 0, NULL);
    }
}

void Renderer_wait(Renderer* pRenderer) {
    vkDeviceWaitIdle(pRenderer->device);
}


void CreateCommandPool(Renderer* pRenderer) {
    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = pRenderer->graphicsFamily;

    if(vkCreateCommandPool(pRenderer->device, &poolInfo, NULL, &pRenderer->commandPool) != VK_SUCCESS) {
        fprintf(stderr, "failed to create command pool");
        exit(EXIT_FAILURE);
    }
}
void CreateCommandBuffers(Renderer* pRenderer) {
    pRenderer->commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * pRenderer->MaxFramesInFlight);

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pRenderer->commandPool;
    allocInfo.commandBufferCount = pRenderer->MaxFramesInFlight;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if(vkAllocateCommandBuffers(pRenderer->device, &allocInfo, pRenderer->commandBuffers) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate frames command buffers");
        exit(EXIT_FAILURE);
    }
}
void CreateDescriptorPool(Renderer* pRenderer) {
    //the used for the sampler2D descriptors
    VkDescriptorPoolSize poolSize1 = {0};
    poolSize1.descriptorCount = 3; //on descriptor for MAX_FRAMES_IN_FLIGHT * descriptors per pool size
    poolSize1.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    uint32_t poolSizesCount = 1;
    VkDescriptorPoolSize poolSizes[] = {poolSize1};

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = 3;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.poolSizeCount = poolSizesCount;

    if(vkCreateDescriptorPool(pRenderer->device, &poolInfo, NULL, &pRenderer->descriptorPool) != VK_SUCCESS) {
        fprintf(stderr, "failed to create descriptor pool");
        exit(EXIT_FAILURE);
    }
}
void CreateAllocator(Renderer* pRenderer) {
    VmaAllocatorCreateInfo allocatorInfo = {0};
    allocatorInfo.device = pRenderer->device;
    allocatorInfo.instance = pRenderer->instance;
    allocatorInfo.physicalDevice = pRenderer->physicalDevice;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;


    if(vmaCreateAllocator(&allocatorInfo, &pRenderer->allocator) != VK_SUCCESS) {
        fprintf(stderr, "failed to create the allocator");
        exit(EXIT_FAILURE);
    }
}
void CreateSampler(Renderer* pRenderer) {
    VkSamplerCreateInfo samplerInfo = {0};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

    if(vkCreateSampler(pRenderer->device, &samplerInfo, NULL, &pRenderer->sampler) != VK_SUCCESS) {
        fprintf(stderr, "failed to create sampelr");
        exit(EXIT_FAILURE);
    }
}
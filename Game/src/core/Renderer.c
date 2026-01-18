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
void CreateOffscreenRenderPass(Renderer* pRenderer);
void CreateColorBuffer(Renderer* pRenderer);
void CreateOffscreenFramebuffers(Renderer* pRenderer);

void Renderer_Init(Renderer* pRenderer) {
    CreateCommandPool(pRenderer);
    CreateCommandBuffers(pRenderer);
    CreateDescriptorPool(pRenderer);
    CreateAllocator(pRenderer);
    CreateSampler(pRenderer);
    CreateOffscreenRenderPass(pRenderer);
    CreateColorBuffer(pRenderer);
    CreateOffscreenFramebuffers(pRenderer);
}
void Renderer_Terminate(Renderer* pRenderer) {
    //destroy stuff that are arrays with the max frames in flight stuff
    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        vmaDestroyImage(pRenderer->allocator, pRenderer->colorBuffer[i], pRenderer->colorBufferAllocation[i]);
        vkDestroyImageView(pRenderer->device, pRenderer->colorBufferView[i], NULL);
        vkDestroyFramebuffer(pRenderer->device, pRenderer->offscreenFramebuffers[i], NULL);
    }
    free(pRenderer->colorBuffer);
    free(pRenderer->colorBufferView);
    free(pRenderer->colorBufferAllocation);
    free(pRenderer->offscreenFramebuffers);

    vkDestroyRenderPass(pRenderer->device, pRenderer->offscreenPass, NULL);


    free(pRenderer->fullscreenSets);
    for(int i = 0; i < MAX_RENDERS; i++) {
        free(pRenderer->textureSets[i]);
    }

    vkDestroyPipelineLayout(pRenderer->device, pRenderer->pipelineLayout, NULL);
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

    VkPushConstantRange range = {0};
    range.offset = 0;
    range.size = sizeof(float) * 16 + sizeof(int) * 2;
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &range;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &pRenderer->singleTexLayout;

    if(vkCreatePipelineLayout(pRenderer->device, &pipelineLayoutInfo, NULL, &pRenderer->pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "failed to create temporary pipeline layout");
        exit(EXIT_FAILURE);
    }

    VkDescriptorSetLayout setLayouts[] = {pRenderer->singleTexLayout, pRenderer->singleTexLayout, pRenderer->singleTexLayout};
    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pRenderer->descriptorPool;
    allocInfo.descriptorSetCount = pRenderer->MaxFramesInFlight;
    allocInfo.pSetLayouts = setLayouts;

    for(int i = 0; i < MAX_RENDERS; i++) {
        pRenderer->textureSets[i] = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * pRenderer->MaxFramesInFlight);
        if(vkAllocateDescriptorSets(pRenderer->device, &allocInfo, pRenderer->textureSets[i]) != VK_SUCCESS) {
            fprintf(stderr, "failed to allocate all single texture descriptor sets");
            exit(EXIT_FAILURE);
        }
        for(int j = 0; j < pRenderer->MaxFramesInFlight; j++) {
            VkDescriptorImageInfo imgInfo = {0};
            imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imgInfo.imageView = GGame->allTextures[i].imageView;
            imgInfo.sampler = pRenderer->sampler;

            VkWriteDescriptorSet setWrite = {0};
            setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.descriptorCount = 1;
            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            setWrite.dstBinding = 0;
            setWrite.pImageInfo = &imgInfo;
            setWrite.dstArrayElement = 0;
            setWrite.dstSet = pRenderer->textureSets[i][j];

            vkUpdateDescriptorSets(pRenderer->device, 1, &setWrite, 0, NULL);
        }
    }



    pRenderer->fullscreenSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * pRenderer->MaxFramesInFlight);
    if(vkAllocateDescriptorSets(pRenderer->device, &allocInfo, pRenderer->fullscreenSets) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate the offscreen to fullscreen texture sets");
        exit(EXIT_FAILURE);
    }
}

void Renderer_StartDraw(Renderer* pRenderer) {
    uint32_t clearValuesCount = 1;
    VkClearValue clearValues[] = {{0.0, 0.0, 0.0, 1.0}};

    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pRenderer->offscreenPass;
    renderPassInfo.clearValueCount = clearValuesCount;
    renderPassInfo.pClearValues = clearValues;
    renderPassInfo.framebuffer = pRenderer->offscreenFramebuffers[currentFrame];
    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = (VkExtent2D){GGame->Width, GGame->Height};

    VkViewport viewport = {0};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = GGame->Width;
	viewport.height = GGame->Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(pRenderer->commandBuffers[currentFrame], 0, 1, &viewport);

	VkRect2D scissor = {0};
	scissor.offset = (VkOffset2D){0, 0};
	scissor.extent = (VkExtent2D){GGame->Width, GGame->Height};
	vkCmdSetScissor(pRenderer->commandBuffers[currentFrame], 0, 1, &scissor);

    vkCmdBeginRenderPass(GGame->m_Renderer.commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void Renderer_EndDraw(Renderer* pRenderer) {
    vkCmdEndRenderPass(GGame->m_Renderer.commandBuffers[currentFrame]);
}

void Renderer_wait(Renderer* pRenderer) {
    vkDeviceWaitIdle(pRenderer->device);
}

void Renderer_RecreateOffscreenFramebuffer(Renderer* pRenderer) {
    //destroy the necessary stuff of the offscreen buffer (color buffer/color view/allocation/framebuffer)
    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        vmaDestroyImage(pRenderer->allocator, pRenderer->colorBuffer[i], pRenderer->colorBufferAllocation[i]);
        vkDestroyImageView(pRenderer->device, pRenderer->colorBufferView[i], NULL);
        vkDestroyFramebuffer(pRenderer->device, pRenderer->offscreenFramebuffers[i], NULL);
    }
    free(pRenderer->colorBuffer);
    free(pRenderer->colorBufferView);
    free(pRenderer->colorBufferAllocation);
    free(pRenderer->offscreenFramebuffers);

    CreateColorBuffer(pRenderer);
    CreateOffscreenFramebuffers(pRenderer);
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
    poolSize1.descriptorCount = 3 * MAX_RENDERS + 3; //on descriptor for MAX_FRAMES_IN_FLIGHT * descriptors per pool size
    poolSize1.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    uint32_t poolSizesCount = 1;
    VkDescriptorPoolSize poolSizes[] = {poolSize1};

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = 3 * MAX_RENDERS + 3;
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
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
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
void CreateOffscreenRenderPass(Renderer* pRenderer) {
	VkAttachmentDescription colorAttachment = {0};
	colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	uint32_t attachmentCount = 1;
	VkAttachmentDescription attachments[] = { colorAttachment };


	VkAttachmentReference colorAttachmentRef = {0};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkSubpassDescription subpass = {0};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {0};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {0};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachmentCount;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(pRenderer->device, &renderPassInfo, NULL, &pRenderer->offscreenPass) != VK_SUCCESS) {
		fprintf(stderr, "failed to create offscreen render pass!");
		exit(EXIT_FAILURE);
	}
}
void CreateColorBuffer(Renderer* pRenderer) {
    pRenderer->colorBuffer = (VkImage*)malloc(sizeof(VkImage) * pRenderer->MaxFramesInFlight);
    pRenderer->colorBufferAllocation = (VmaAllocation*)malloc(sizeof(VmaAllocation) * pRenderer->MaxFramesInFlight);
    pRenderer->colorBufferView = (VkImageView*)malloc(sizeof(VkImageView) * pRenderer->MaxFramesInFlight);

    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = 1;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.extent.width = GGame->Width;
    imageInfo.extent.height = GGame->Height;
    imageInfo.extent.depth = 1;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;

    VmaAllocationCreateInfo allocInfo = {0};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkImageViewCreateInfo imageViewInfo = {0};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;

    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        if(vmaCreateImage(pRenderer->allocator, &imageInfo, &allocInfo, &pRenderer->colorBuffer[i], &pRenderer->colorBufferAllocation[i], NULL) != VK_SUCCESS) {
            fprintf(stderr, "failed to create color buffer image");
            exit(EXIT_FAILURE);
        }

        imageViewInfo.image = pRenderer->colorBuffer[i];

        if(vkCreateImageView(pRenderer->device, &imageViewInfo, NULL, &pRenderer->colorBufferView[i]) != VK_SUCCESS) {
            fprintf(stderr, "failed to create color buffer image view");
            exit(EXIT_FAILURE);
        }
    }
}
void CreateOffscreenFramebuffers(Renderer* pRenderer) {
    pRenderer->offscreenFramebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * pRenderer->MaxFramesInFlight);

    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        uint32_t attachmentCount = 1;
        VkImageView attachments[] = {pRenderer->colorBufferView[i]};

        VkFramebufferCreateInfo framebufferInfo = {0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pRenderer->offscreenPass;
		framebufferInfo.attachmentCount = attachmentCount;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = GGame->Width;
		framebufferInfo.height = GGame->Height;
		framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(pRenderer->device, &framebufferInfo, NULL, &pRenderer->offscreenFramebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "failed to initialize offscreen framebuffer");
            exit(EXIT_FAILURE);
        }
    }
}

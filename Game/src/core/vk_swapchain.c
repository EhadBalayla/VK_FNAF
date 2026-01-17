#include "vk_swapchain.h"

#include "vk_context.h" //literally only for the SwapchainSupportDetails

#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <limits.h>

uint32_t imageIndex = 0;

//small helper function
uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
    if(value < min) value = min;
    else if (value > max) value = max;

    return value;
}
 

//creation function declarations
void CreateSwapchain(vk_swapchain* pSwapchain);
void CreateRenderPass(vk_swapchain* pSwapchain);
void CreateImageViews(vk_swapchain* pSwapchain);
void CreateFramebuffers(vk_swapchain* pSwapchain);
void CreateSwapSyncObjects(vk_swapchain* pSwapchain);

//declarations of helper functions
VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats, uint32_t formatsCount);
VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* availableModes, uint32_t modesCount);
VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR* capabilities, void* pWindow);



void VKSwapchain_Create(vk_swapchain* pSwapchain) {
    CreateSwapchain(pSwapchain);
	CreateRenderPass(pSwapchain);
    CreateImageViews(pSwapchain);
	CreateFramebuffers(pSwapchain);
	CreateSwapSyncObjects(pSwapchain);
}
void VKSwapchain_Delete(vk_swapchain* pSwapchain) {
    vk_context* context = (vk_context*)pSwapchain->pContext;
    
	for(int i = 0; i < pSwapchain->MaxFramesInFlight; i++) {
		vkDestroySemaphore(context->device, pSwapchain->imageAvailableSemaphores[i], NULL);
		vkDestroyFence(context->device, pSwapchain->inFlightFences[i], NULL);
	}
	free(pSwapchain->imageAvailableSemaphores);
	free(pSwapchain->inFlightFences);

    for(int i = 0; i < pSwapchain->swapchainImagesCount; i++) {
		vkDestroyFramebuffer(context->device, pSwapchain->swapchainFramebuffers[i], NULL);
        vkDestroyImageView(context->device, pSwapchain->swapchainImageViews[i], NULL);
		vkDestroySemaphore(context->device, pSwapchain->renderingFinishedSemaphores[i], NULL);
    }
	free(pSwapchain->swapchainFramebuffers);
    free(pSwapchain->swapchainImageViews);
	free(pSwapchain->renderingFinishedSemaphores);

	vkDestroyRenderPass(context->device, pSwapchain->swapchainRenderPass, NULL);

    free(pSwapchain->swapchainImages);
    vkDestroySwapchainKHR(context->device, pSwapchain->swapchain, NULL);
}

void VKSwapchain_Recreate(vk_swapchain* pSwapchain) {
	vk_context* context = (vk_context*)pSwapchain->pContext;

	vkDeviceWaitIdle(context->device);

	//destroy the necessary stuff except the renderpass and sync objects
    for(int i = 0; i < pSwapchain->swapchainImagesCount; i++) {
		vkDestroyFramebuffer(context->device, pSwapchain->swapchainFramebuffers[i], NULL);
        vkDestroyImageView(context->device, pSwapchain->swapchainImageViews[i], NULL);
    }
	free(pSwapchain->swapchainFramebuffers);
    free(pSwapchain->swapchainImageViews);

    free(pSwapchain->swapchainImages);
    vkDestroySwapchainKHR(context->device, pSwapchain->swapchain, NULL);

	//recreate the necessary stuff except the renderpass and sync objects
	CreateSwapchain(pSwapchain);
	CreateImageViews(pSwapchain);
	CreateFramebuffers(pSwapchain);
}



void CreateSwapchain(vk_swapchain* pSwapchain) {
    vk_context* context = (vk_context*)pSwapchain->pContext;

    SwapChainSupportDetails swapchainSupport = QuerySwapchainSupport(context->physicalDevice, context);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats, swapchainSupport.formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes, swapchainSupport.presentModesCount);
    VkExtent2D extent = chooseSwapExtent(&swapchainSupport.capabilities, pSwapchain->pWindow);

    free(swapchainSupport.formats);
    free(swapchainSupport.presentModes);

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

    VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = context->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndicies[] = { context->graphicsFamily, context->presentFamily};
	if (queueFamilyIndicies[0] != queueFamilyIndicies[1]) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndicies;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
	}

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(context->device, &createInfo, NULL, &pSwapchain->swapchain) != VK_SUCCESS) {
		fprintf(stderr, "failed to create swapchain");
	}

    vkGetSwapchainImagesKHR(context->device, pSwapchain->swapchain, &pSwapchain->swapchainImagesCount, NULL);
    pSwapchain->swapchainImages = (VkImage*)malloc(sizeof(VkImage) * pSwapchain->swapchainImagesCount);
    vkGetSwapchainImagesKHR(context->device, pSwapchain->swapchain, &pSwapchain->swapchainImagesCount, pSwapchain->swapchainImages);

    pSwapchain->swapchainImageFormat = surfaceFormat.format;
    pSwapchain->swapchainImageExtent = extent;
}
void CreateImageViews(vk_swapchain* pSwapchain) {
    vk_context* context = (vk_context*)pSwapchain->pContext;

    pSwapchain->swapchainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * pSwapchain->swapchainImagesCount);
    
    VkImageViewCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = pSwapchain->swapchainImageFormat;

	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

    for(int i = 0; i < pSwapchain->swapchainImagesCount; i++) {
        createInfo.image = pSwapchain->swapchainImages[i];
        if(vkCreateImageView(context->device, &createInfo, NULL, &pSwapchain->swapchainImageViews[i]) != VK_SUCCESS) {
            fprintf(stderr, "failed to create an image view for one of the swapchain images");
            exit(EXIT_FAILURE);
        }
    }
}
void CreateRenderPass(vk_swapchain* pSwapchain) {
	vk_context* context = (vk_context*)pSwapchain->pContext;

	VkAttachmentDescription colorAttachment = {0};
	colorAttachment.format = pSwapchain->swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

	if (vkCreateRenderPass(context->device, &renderPassInfo, NULL, &pSwapchain->swapchainRenderPass) != VK_SUCCESS) {
		fprintf(stderr, "failed to create swapchain render pass!");
		exit(EXIT_FAILURE);
	}
}
void CreateFramebuffers(vk_swapchain* pSwapchain) {
	vk_context* context = (vk_context*)pSwapchain->pContext;

	pSwapchain->swapchainFramebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * pSwapchain->swapchainImagesCount);

	for(int i = 0; i < pSwapchain->swapchainImagesCount; i++) {
		uint32_t attachmentCount = 1;
		VkImageView attachments[] = {pSwapchain->swapchainImageViews[i]};

		VkFramebufferCreateInfo createInfo = {0};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = pSwapchain->swapchainRenderPass;
		createInfo.attachmentCount = attachmentCount;
		createInfo.pAttachments = attachments;
		createInfo.width = pSwapchain->swapchainImageExtent.width;
		createInfo.height = pSwapchain->swapchainImageExtent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(context->device, &createInfo, NULL, &pSwapchain->swapchainFramebuffers[i]) != VK_SUCCESS) {
			fprintf(stderr, "couldn't create framebuffer");
			exit(EXIT_FAILURE);
		}
	}
}
void CreateSwapSyncObjects(vk_swapchain* pSwapchain) {
	vk_context* context = (vk_context*)pSwapchain->pContext;

	pSwapchain->imageAvailableSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * pSwapchain->MaxFramesInFlight);
	pSwapchain->inFlightFences = (VkFence*)malloc(sizeof(VkFence) * pSwapchain->MaxFramesInFlight);
	pSwapchain->renderingFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * pSwapchain->swapchainImagesCount);

	VkSemaphoreCreateInfo semaphoreInfo = {0};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {0};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < pSwapchain->MaxFramesInFlight; i++) {
		if (vkCreateSemaphore(context->device, &semaphoreInfo, NULL, &pSwapchain->imageAvailableSemaphores[i]) != VK_SUCCESS |
			vkCreateFence(context->device, &fenceInfo, NULL, &pSwapchain->inFlightFences[i]) != VK_SUCCESS) {
			fprintf(stderr, "failed to create first synchronization objects of the swapchain for a frame");
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < pSwapchain->swapchainImagesCount; i++) {
		if (vkCreateSemaphore(context->device, &semaphoreInfo, NULL, &pSwapchain->renderingFinishedSemaphores[i]) != VK_SUCCESS) {
			fprintf(stderr, "failed to create first synchronization objects of the swapchain for a frame");
			exit(EXIT_FAILURE);
		}
	}
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats, uint32_t formatsCount) {
    for(int i = 0; i < formatsCount; i++) {
        if(availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}
VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* availableModes, uint32_t modesCount) {
    for (int i = 0; i < modesCount; i++) {
		if (availableModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			return availableModes[i];
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR* capabilities, void* pWindow) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
		return capabilities->currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize((GLFWwindow*)pWindow, &width, &height);

		VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };

		actualExtent.width = clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
		actualExtent.height = clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

		return actualExtent;
	}
}

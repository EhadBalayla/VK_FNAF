#include "Renderer.h"

#include <stdio.h>
#include <stdlib.h>

int currentFrame = 0;

//creation functions declarations
void CreateCommandPool(Renderer* pRenderer);
void CreateCommandBuffers(Renderer* pRenderer);
void CreateSyncObjects(Renderer* pRenderer);

void Renderer_Init(Renderer* pRenderer) {
    CreateCommandPool(pRenderer);
    CreateCommandBuffers(pRenderer);
    CreateSyncObjects(pRenderer);
}
void Renderer_Terminate(Renderer* pRenderer) {
    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        vkDestroySemaphore(pRenderer->device, pRenderer->renderingFinishedSemaphores[i], NULL);
        vkDestroyFence(pRenderer->device, pRenderer->inFlightFences[i], NULL);
    }
    vkFreeCommandBuffers(pRenderer->device, pRenderer->commandPool, pRenderer->MaxFramesInFlight, pRenderer->commandBuffers);

    free(pRenderer->inFlightFences);
    free(pRenderer->renderingFinishedSemaphores);
    free(pRenderer->commandBuffers);

    vkDestroyCommandPool(pRenderer->device, pRenderer->commandPool, NULL);
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
void CreateSyncObjects(Renderer* pRenderer) {
    pRenderer->renderingFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * pRenderer->MaxFramesInFlight);
    pRenderer->inFlightFences = (VkFence*)malloc(sizeof(VkFence) * pRenderer->MaxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo = {0};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {0};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < pRenderer->MaxFramesInFlight; i++) {
        if (vkCreateSemaphore(pRenderer->device, &semaphoreInfo, NULL, &pRenderer->renderingFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(pRenderer->device, &fenceInfo, NULL, &pRenderer->inFlightFences[i]) != VK_SUCCESS) {

			fprintf(stderr, "failed to create synchronization objects for a frame!");
            exit(EXIT_FAILURE);
		}
    }
}
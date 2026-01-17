#include "Window.h"
#include <GLFW/glfw3.h>

#include "Game.h"

#include <stdio.h>
#include <stdlib.h>

void Window_InitGLFW() {
    if(glfwInit() != GLFW_TRUE) {
        fprintf(stderr, "failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }
}
void Window_TerminateGLFW() {
    glfwTerminate();
}

void Window_CreateWindow(Window* pWindow, int Width, int Height, const char* title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    pWindow->m_GLFWwindow = glfwCreateWindow(Width, Height, title, NULL, NULL);
}
void Window_DestroyWindow(Window* pWindow) {
    glfwDestroyWindow((GLFWwindow*)pWindow->m_GLFWwindow);
}

void Window_CreateContext(Window* pWindow) {
    pWindow->m_Context.pWindow = pWindow->m_GLFWwindow;
    VKContext_InitGPU(&pWindow->m_Context);

    pWindow->m_Swapchain.pContext = &pWindow->m_Context;
    pWindow->m_Swapchain.pWindow = pWindow->m_GLFWwindow;
    pWindow->m_Swapchain.MaxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    VKSwapchain_Create(&pWindow->m_Swapchain);
}
void Window_DestroyContext(Window* pWindow) {
    VKSwapchain_Delete(&pWindow->m_Swapchain);

    VKContext_TerminateGPU(&pWindow->m_Context);
}

void Window_StartFrame(Window* pWindow) {
    vkWaitForFences(pWindow->m_Context.device, 1, &GGame->m_Renderer.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(pWindow->m_Context.device, 1, &GGame->m_Renderer.inFlightFences[currentFrame]);

    vkAcquireNextImageKHR(pWindow->m_Context.device, pWindow->m_Swapchain.swapchain, UINT64_MAX, pWindow->m_Swapchain.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(GGame->m_Renderer.commandBuffers[currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(GGame->m_Renderer.commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "failed to start current frame's command buffer");
        exit(EXIT_FAILURE);
    }

    uint32_t clearValuesCount = 1;
    VkClearValue clearValues[] = {{0.0, 0.0, 0.0, 1.0}};

    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pWindow->m_Swapchain.swapchainRenderPass;
    renderPassInfo.clearValueCount = clearValuesCount;
    renderPassInfo.pClearValues = clearValues;
    renderPassInfo.framebuffer = pWindow->m_Swapchain.swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset.x = 0; renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = pWindow->m_Swapchain.swapchainImageExtent;

    VkViewport viewport = {0};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = pWindow->m_Swapchain.swapchainImageExtent.width;
	viewport.height = pWindow->m_Swapchain.swapchainImageExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(GGame->m_Renderer.commandBuffers[currentFrame], 0, 1, &viewport);

	VkRect2D scissor = {0};
	scissor.offset.x = 0; scissor.offset.y = 0;
	scissor.extent = pWindow->m_Swapchain.swapchainImageExtent;
	vkCmdSetScissor(GGame->m_Renderer.commandBuffers[currentFrame], 0, 1, &scissor);

    vkCmdBeginRenderPass(GGame->m_Renderer.commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

}
void Window_EndFrame(Window* pWindow) {
    vkCmdEndRenderPass(GGame->m_Renderer.commandBuffers[currentFrame]);

    if(vkEndCommandBuffer(GGame->m_Renderer.commandBuffers[currentFrame]) != VK_SUCCESS) {
        fprintf(stderr, "failed to end current frame's command buffer");
        exit(EXIT_FAILURE);
    }

    VkSubmitInfo submitInfo = {0};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &pWindow->m_Swapchain.imageAvailableSemaphores[currentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &GGame->m_Renderer.commandBuffers[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &GGame->m_Renderer.renderingFinishedSemaphores[imageIndex];

	if (vkQueueSubmit(pWindow->m_Context.graphicsQueue, 1, &submitInfo, GGame->m_Renderer.inFlightFences[currentFrame]) != VK_SUCCESS) {
		fprintf(stderr, "failed to submit draw command buffer");
        exit(EXIT_FAILURE);
	}
	VkPresentInfoKHR presentInfo = {0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &GGame->m_Renderer.renderingFinishedSemaphores[imageIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &pWindow->m_Swapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(pWindow->m_Context.PresentQueue, &presentInfo);
	
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
void Window_PollEvents() {
    glfwPollEvents();
}

int Window_ShouldClose(Window* pWindow) {
    return glfwWindowShouldClose((GLFWwindow*)pWindow->m_GLFWwindow);
}


void Window_SetResizeCallback(Window* pWindow, void (*resizefunc)(GLFWwindow* GLFWwindow, int width, int height)) {
    glfwSetFramebufferSizeCallback((GLFWwindow*)pWindow->m_GLFWwindow, resizefunc);
}
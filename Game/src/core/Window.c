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
    vkWaitForFences(pWindow->m_Context.device, 1, &pWindow->m_Swapchain.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(pWindow->m_Context.device, 1, &pWindow->m_Swapchain.inFlightFences[currentFrame]);

    vkAcquireNextImageKHR(pWindow->m_Context.device, pWindow->m_Swapchain.swapchain, UINT64_MAX, pWindow->m_Swapchain.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(GGame->m_Renderer.commandBuffers[currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(GGame->m_Renderer.commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "failed to start current frame's command buffer");
        exit(EXIT_FAILURE);
    }

}
void Window_EndFrame(Window* pWindow) {
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
	submitInfo.pSignalSemaphores = &pWindow->m_Swapchain.renderingFinishedSemaphores[imageIndex];

	if (vkQueueSubmit(pWindow->m_Context.graphicsQueue, 1, &submitInfo, pWindow->m_Swapchain.inFlightFences[currentFrame]) != VK_SUCCESS) {
		fprintf(stderr, "failed to submit draw command buffer");
        exit(EXIT_FAILURE);
	}
	VkPresentInfoKHR presentInfo = {0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &pWindow->m_Swapchain.renderingFinishedSemaphores[imageIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &pWindow->m_Swapchain.swapchain;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(pWindow->m_Context.PresentQueue, &presentInfo);
	
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
void Window_PollEvents() {
    glfwPollEvents();
}

void Window_StartScreen(Window* pWindow) {

    VkImageMemoryBarrier imageBarrier = {0};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = GGame->m_Renderer.colorBuffer[currentFrame];
    imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange.baseMipLevel = 0;
    imageBarrier.subresourceRange.levelCount = 1;
    imageBarrier.subresourceRange.baseArrayLayer = 0;
    imageBarrier.subresourceRange.layerCount = 1;
    imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkPipelineStageFlags srcMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    vkCmdPipelineBarrier(GGame->m_Renderer.commandBuffers[currentFrame], 
    srcMask, dstMask,
    0, 
    0, NULL,
    0, NULL,
    1, &imageBarrier);



    VkDescriptorImageInfo imageInfo = {0};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = GGame->m_Renderer.colorBufferView[currentFrame];
    imageInfo.sampler = GGame->m_Renderer.sampler;

    VkWriteDescriptorSet setWrite = {0};
    setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setWrite.descriptorCount = 1;
    setWrite.dstSet = GGame->m_Renderer.fullscreenSets[currentFrame];
    setWrite.pImageInfo = &imageInfo;
    setWrite.dstBinding = 0;
    setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    vkUpdateDescriptorSets(GGame->m_Renderer.device, 1, &setWrite, 0, NULL);



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

    vkCmdBindDescriptorSets(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->m_Renderer.pipelineLayout, 0, 1, &GGame->m_Renderer.fullscreenSets[currentFrame], 0, NULL);
    vkCmdBindPipeline(GGame->m_Renderer.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GGame->FullscreenShader.graphicsPipeline);
    vkCmdDraw(GGame->m_Renderer.commandBuffers[currentFrame], 6, 1, 0, 0);
}
void Window_EndScreen(Window* pWindow) {
    vkCmdEndRenderPass(GGame->m_Renderer.commandBuffers[currentFrame]);
}

int Window_ShouldClose(Window* pWindow) {
    return glfwWindowShouldClose((GLFWwindow*)pWindow->m_GLFWwindow);
}


void Window_SetResizeCallback(Window* pWindow, void (*resizefunc)(GLFWwindow* GLFWwindow, int width, int height)) {
    glfwSetFramebufferSizeCallback((GLFWwindow*)pWindow->m_GLFWwindow, resizefunc);
}
void Window_SetKeyCallback(Window* pWindow, void (*GLFWkeyfun)(GLFWwindow *window, int key, int scancode, int action, int mods)) {
    glfwSetKeyCallback(pWindow->m_GLFWwindow, GLFWkeyfun);
}
void Window_SetMouseCallback(Window* pWindow, void (*GLFWcursorposfun)(GLFWwindow *window, double xpos, double ypos)) {
    glfwSetCursorPosCallback(pWindow->m_GLFWwindow, GLFWcursorposfun);
}
void Window_SetMouseButtonCallback(Window* pWindow, void (*GLFWmousebuttonfun)(GLFWwindow *window, int button, int action, int mods)) {
    glfwSetMouseButtonCallback(pWindow->m_GLFWwindow, GLFWmousebuttonfun);
}
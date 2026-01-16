#ifndef WINDOW
#define WINDOW

#include "vk_context.h"
#include "vk_swapchain.h"

typedef struct {
    void* m_GLFWwindow;
    vk_context m_Context;
    vk_swapchain m_Swapchain;
} Window;

void Window_InitGLFW();
void Window_TerminateGLFW();

void Window_CreateWindow(Window* pWindow, int Width, int Height, const char* title);
void Window_DestroyWindow(Window* pWindow);

void Window_CreateContext(Window* pWindow);
void Window_DestroyContext(Window* pWindow);

void Window_StartFrame(Window* pWindow);
void Window_EndFrame(Window* pWindow);
void Window_PollEvents();

int Window_ShouldClose(Window* pWindow);

typedef struct GLFWwindow GLFWwindow;
void Window_SetResizeCallback(Window* pWindow, void (*GLFWframebuffersizefun)(GLFWwindow* GLFWwindow, int width, int height));

#endif
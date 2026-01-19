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

void Window_StartScreen(Window* pWindow);
void Window_EndScreen(Window* pWindow);

int Window_ShouldClose(Window* pWindow);

typedef struct GLFWwindow GLFWwindow;
void Window_SetResizeCallback(Window* pWindow, void (*GLFWframebuffersizefun)(GLFWwindow* GLFWwindow, int width, int height));
void Window_SetKeyCallback(Window* pWindow, void (*GLFWkeyfun)(GLFWwindow *window, int key, int scancode, int action, int mods));
void Window_SetMouseCallback(Window* pWindow, void (*GLFWcursorposfun)(GLFWwindow *window, double xpos, double ypos));
void Window_SetMouseButtonCallback(Window* pWindow, void (*GLFWmousebuttonfun)(GLFWwindow *window, int button, int action, int mods));

#endif
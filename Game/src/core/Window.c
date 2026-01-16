#include "Window.h"
#include <GLFW/glfw3.h>

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
    VKContext_InitGPU(&pWindow->m_Context);
}
void Window_DestroyContext(Window* pWindow) {
    VKContext_TerminateGPU(&pWindow->m_Context);
}

void Window_StartFrame(Window* pWindow) {

}
void Window_EndFrame(Window* pWindow) {

}
void Window_PollEvents() {
    glfwPollEvents();
}

int Window_ShouldClose(Window* pWindow) {
    return glfwWindowShouldClose((GLFWwindow*)pWindow->m_GLFWwindow);
}
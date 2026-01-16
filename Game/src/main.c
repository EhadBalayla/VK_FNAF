#include <stdio.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "temp Window", NULL, NULL);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
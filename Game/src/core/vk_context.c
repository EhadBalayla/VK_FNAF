#include "vk_context.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);
        }

        return VK_FALSE;
}

int validationLayersCount = 1;
const char* validationLayers[] = {
   "VK_LAYER_KHRONOS_validation"
} ;

#ifdef NDEBUG
    const int enableValidationLayers = 0;
#else
    const int enableValidationLayers = 1;
#endif


//creation function declarations
void CreateInstance(vk_context* pContext);
void CreateDebugMessenger(vk_context* pContext);
void CreateSurface(vk_context* pContext);
void CreatePhysicalDevice(vk_context* pContext);
void CreateLogicalDevice(vk_context* pContext);

//helper function declarations
int checkValidationLayerSupport();
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);


void VKContext_InitGPU(vk_context* pContext) {
    CreateInstance(pContext);
    CreateDebugMessenger(pContext);
    CreateSurface(pContext);
    CreatePhysicalDevice(pContext);
    CreateLogicalDevice(pContext);
}
void VKContext_TerminateGPU(vk_context* pContext) {
    if(enableValidationLayers) DestroyDebugUtilsMessengerEXT(pContext->instance, pContext->debugMessenger, NULL);

    vkDestroyInstance(pContext->instance, NULL);
}




void CreateInstance(vk_context* pContext) {
    if(enableValidationLayers && !checkValidationLayerSupport()) {
        fprintf(stderr, "validation layers requested but not available");
        exit(EXIT_FAILURE);
    }

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VK_FNAF";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo = {0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    char** allExtensions;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallback;

    if(enableValidationLayers) {
        const char* debugExtensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        allExtensions = (char**)malloc(sizeof(char*) * (glfwExtensionCount + 1));
        memcpy(allExtensions, glfwExtensions, sizeof(char*) * glfwExtensionCount);
        memcpy(allExtensions + glfwExtensionCount, debugExtensions, sizeof(char*));

        instanceInfo.enabledExtensionCount = glfwExtensionCount + 1;
        instanceInfo.ppEnabledExtensionNames = (const char* const*)allExtensions;

        instanceInfo.enabledLayerCount = validationLayersCount;
        instanceInfo.ppEnabledLayerNames = validationLayers;

        instanceInfo.pNext = &debugInfo;
    }
    else {
        instanceInfo.enabledExtensionCount = glfwExtensionCount;
        instanceInfo.ppEnabledExtensionNames = glfwExtensions;

        instanceInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&instanceInfo, NULL, &pContext->instance) != VK_SUCCESS) {
        fprintf(stderr, "failed to create Vulkan instance");
        exit(EXIT_FAILURE);
    }

    if(enableValidationLayers) free(allExtensions); //if we do have validation layers, that means that we do allocate the pointer, so if we do then dont forget to free at the end
}
void CreateDebugMessenger(vk_context* pContext) {
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    if(CreateDebugUtilsMessengerEXT(pContext->instance, &createInfo, NULL, &pContext->debugMessenger) != VK_SUCCESS) {
        fprintf(stderr, "failed to create debug messenger");
        exit(EXIT_FAILURE);
    }
}
void CreateSurface(vk_context* pContext) {

}
void CreatePhysicalDevice(vk_context* pContext) {

}
void CreateLogicalDevice(vk_context* pContext) {

}

int checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);


    for(int i = 0; i < validationLayersCount; i++) {
        int layerFound = 0;

        for(int j = 0; j < layerCount; j++) {
            if(strcmp(validationLayers[i], availableLayers[j].layerName) == 0) {
                layerFound = 1;
                break;
            }
        }

        if(!layerFound) {
            free(availableLayers); //since it is an array we allocated, we need to free it before returning
            return 0;
        }
    }

    free(availableLayers); //same thing, we allocated this array so we need to free it before returning
    return 1;
}
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}
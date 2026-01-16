#include "vk_context.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MAX_FRAMES_IN_FLIGHT = 3;

int QueueFamilyIndicies_IsComplete(QueueFamilyIndicies indicies) {
    return indicies.hasGraphics && indicies.hasPresent;
}



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
};

int deviceExtensionsCount = 1;
const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

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
int checkValidationLayerSupport();VkPhysicalDeviceProperties deviceProperties;
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
int IsDeviceSuitable(VkPhysicalDevice device, vk_context* pContext);
int checkDeviceExtensionSupport(VkPhysicalDevice device);



void VKContext_InitGPU(vk_context* pContext) {
    CreateInstance(pContext);
    CreateDebugMessenger(pContext);
    CreateSurface(pContext);
    CreatePhysicalDevice(pContext);
    CreateLogicalDevice(pContext);
}
void VKContext_TerminateGPU(vk_context* pContext) {
    vkDestroyDevice(pContext->device, NULL);

    vkDestroySurfaceKHR(pContext->instance, pContext->surface, NULL);

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
    if(glfwCreateWindowSurface(pContext->instance, (GLFWwindow*)pContext->pWindow, NULL, &pContext->surface) != VK_SUCCESS) {
        fprintf(stderr, "failed to create window surface");
        exit(EXIT_FAILURE);
    }
}
void CreatePhysicalDevice(vk_context* pContext) {
    pContext->physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(pContext->instance, &deviceCount, NULL);

    if(deviceCount == 0) {
        fprintf(stderr, "failed to find GPUs with Vulkan support");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(pContext->instance, &deviceCount, devices);


    for(int i = 0; i < deviceCount; i++) {
        if(IsDeviceSuitable(devices[i], pContext)) {
            pContext->physicalDevice = devices[i];
            break;
        }
    }

    if(pContext->physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "failed to find a suitable GPU");
        exit(EXIT_FAILURE);
    }
}
void CreateLogicalDevice(vk_context* pContext) {
    int queueInfosCount = 2;
    VkDeviceQueueCreateInfo queueCreateInfos[] = { {0}, {0} };
    if(pContext->graphicsFamily == pContext->presentFamily) {
        queueInfosCount = 1;
        queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[0].queueFamilyIndex = pContext->graphicsFamily;
        queueCreateInfos[0].queueCount = 1;
    
        float QueuePriority = 1.0f;
        queueCreateInfos[0].pQueuePriorities = &QueuePriority;
    } 
    else {
        uint32_t indicies[] = {pContext->graphicsFamily, pContext->presentFamily};
        for(int i = 0; i < queueInfosCount; i++) {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = indicies[i];
            queueCreateInfos[i].queueCount = 1;
            
            float QueuePriority = 1.0f;
            queueCreateInfos[i].pQueuePriorities = &QueuePriority;
        }
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceInfo = {0};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queueCreateInfos;
    deviceInfo.queueCreateInfoCount = queueInfosCount;
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;
    if(enableValidationLayers) {
        deviceInfo.enabledLayerCount = validationLayersCount;
        deviceInfo.ppEnabledLayerNames = validationLayers;
    } 
    else {
        deviceInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(pContext->physicalDevice, &deviceInfo, NULL, &pContext->device) != VK_SUCCESS) {
        fprintf(stderr, "failed to create the logical device");
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(pContext->device, pContext->graphicsFamily, 0, &pContext->graphicsQueue);
    vkGetDeviceQueue(pContext->device, pContext->presentFamily, 0, &pContext->PresentQueue);
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
int IsDeviceSuitable(VkPhysicalDevice device, vk_context* pContext) {
    QueueFamilyIndicies indicies = FindQueueFamilies(device, pContext);

    int swapchainAdequate = 0;
    int extensionsSupported = checkDeviceExtensionSupport(device);
    if(extensionsSupported) {
        SwapChainSupportDetails swapchainSupport = QuerySwapchainSupport(device, pContext);
        swapchainAdequate = swapchainSupport.formatCount > 0 && swapchainSupport.presentModesCount > 0;

        free(swapchainSupport.formats);
        free(swapchainSupport.presentModes);
    }

    if(QueueFamilyIndicies_IsComplete(indicies) && extensionsSupported && swapchainAdequate) {
        pContext->graphicsFamily = indicies.graphicsFamily;
        pContext->presentFamily = indicies.presentFamily;
        return 1;
    }

    return 0;
}
QueueFamilyIndicies FindQueueFamilies(VkPhysicalDevice device, vk_context* pContext) {
    QueueFamilyIndicies indicies = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for(int i = 0; i < queueFamilyCount; i++) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indicies.graphicsFamily = i;
            indicies.hasGraphics = 1;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, pContext->surface, &presentSupport);

        if(presentSupport) {
            indicies.presentFamily = i;
            indicies.hasPresent = 1;
        }

        if(QueueFamilyIndicies_IsComplete(indicies)) {
            break;
        }
    }

    free(queueFamilies);
    return indicies;
}
int checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);


    for(int i = 0; i < extensionCount; i++) {
        if(strcmp(availableExtensions[i].extensionName, deviceExtensions[0]) == 0) {
            return 1;
        }
    }

    return 0;
}
SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, vk_context* pContext) {  
    SwapChainSupportDetails details = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, pContext->surface, &details.capabilities);


    vkGetPhysicalDeviceSurfaceFormatsKHR(device, pContext->surface, &details.formatCount, NULL);
    if(details.formatCount != 0) {
        details.formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * details.formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, pContext->surface, &details.formatCount, details.formats);
    }


    vkGetPhysicalDeviceSurfacePresentModesKHR(device, pContext->surface, &details.presentModesCount, NULL);

    if (details.presentModesCount != 0) {
        details.presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * details.presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, pContext->surface, &details.presentModesCount, details.presentModes);
    }

    return details;
}
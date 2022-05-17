#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>

using std::vector;

#define VK_CHECK(result)                                   \
    VkResult err = result;                                 \
    if (err)                                               \
    {                                                      \
        std::cerr << "Vulkan Error: " << err << std::endl; \
        abort();                                           \
    }

namespace engine
{
    namespace vulkan
    {
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData)
        {
            std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }

        struct InstanceCreateData
        {
            vk::ApplicationInfo appInfo;
            bool enableValidationLayers;
            vector<const char *> requiredExtensions;
            vector<const char *> validationLayers;
        };

        struct QueueFamilyIndices
        {
            uint32_t graphics;
            uint32_t presentation;
        };

        static const char *ENGINE_NAME = "Vulkan";
        static const int ENINGE_VERSION[3] = {1, 0, 0};

        static const vector<const char *> DEVICE_EXTENSIONS{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        static const vector<const char *> VALIDATION_LAYERS = {
            "VK_LAYER_KHRONOS_validation"};
    }
}

#endif
#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <limits>
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
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            std::cerr << "\033[32m [VALIDATION LAYER] " << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }

        /**
         * @brief Holds the data which is used when creating a Vulkan Instance
         *
         * @param appInfo The Vulkan application info object holding app name,
         * version etc.
         * @param enableValidationLayers Whether to enable validation layers for
         * debugging. If set to true Vulkan debug messenger object will be initialized
         * @param requiredExtensions Required instance extensions. (Eg: GLFW/SDL
         * extensions, VK_EXT_debug_utils etc.)
         * @param validationLayers the necessary validation layers for debugging
         */
        struct InstanceCreateData
        {
            vk::ApplicationInfo appInfo;
            bool enableValidationLayers;
            vector<const char*> requiredExtensions;
            vector<const char*> validationLayers;
        };

        struct QueueFamilyIndices
        {
            uint32_t graphics = std::numeric_limits<unsigned int>::max();
            uint32_t presentation = std::numeric_limits<unsigned int>::max();

            inline bool isGraphicsSupported() const
            {
                return graphics != std::numeric_limits<unsigned int>::max();
            }

            inline bool isPresentationSupported() const
            {
                return presentation != std::numeric_limits<unsigned int>::max();
            }

            // Set is used so that each value is unique. Graphics and presenstation
            // queue family can refer to the same thing, so this step is necessary
            inline std::set<uint32_t> getIndices() const
            {
                return std::set<uint32_t>({ graphics, presentation });
            }
        };

        struct SwapchainSupportInfo
        {
            vk::SurfaceCapabilitiesKHR capabilities;
            vector<vk::SurfaceFormatKHR> surfaceFormats;
            vector<vk::PresentModeKHR> presentModes;

            inline bool isSupported() const
            {
                return !surfaceFormats.empty() && !presentModes.empty();
            }
        };

        struct SwapchainInitData
        {
            vk::SurfaceFormatKHR surfaceFormat;
            vk::PresentModeKHR presentMode;
            vk::Extent2D extent;
        };

        struct SwapchainData
        {
            vk::SwapchainKHR swapchain;
            vk::Format imageFormat;
            vector<vk::Image> images;
            vector<vk::ImageView> imageViews;
        };

        static const char* ENGINE_NAME = "Vulkan";
        static const int ENINGE_VERSION[3] = { 1, 0, 0 };

        static const vector<const char*> DEVICE_EXTENSIONS{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        static const vector<const char*> VALIDATION_LAYERS = {
            "VK_LAYER_KHRONOS_validation" };
    }
}

#endif
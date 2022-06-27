#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
#include <limits>
#include <utility>

using std::vector;

#define VK_HANDLE_RESULT(f) \
    { \
        vk::Result result = f; \
        if(result != vk::Result::eSuccess) \
        { \
            std::cerr << "\033[31m [VULKAN RESULT] " << result << std::endl; \
            std::cerr << "\033[0m"; \
            abort(); \
        } \
    } \


namespace engine
{
    namespace vulkan
    {
        static void handleVulkanException()
        {
            try
            {
                throw;
            }
            catch (vk::SystemError& err)
            {
                std::cerr << "\033[31m [VULKAN EXCEPTION] " << err.what() << std::endl;
                std::cerr << "\033[0m";
            }
            catch (std::exception& err)
            {
                std::cerr << "\033[31m [EXCEPTION] " << err.what() << std::endl;
                std::cerr << "\033[0m";
            }
            catch (...)
            {
                std::cerr << "\033[31m [UNKNOWN EXCEPTION]" << std::endl;
                std::cerr << "\033[0m";
            }

        }


        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            switch (messageSeverity)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                std::cerr << "\033[32m [VALIDATION LAYER][VERBOSE] " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                std::cerr << "\033[32m [VALIDATION LAYER][INFO] " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                std::cerr << "\033[33m [VALIDATION LAYER][WARN] " << pCallbackData->pMessage << std::endl;
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cerr << "\033[31m [VALIDATION LAYER][ERROR] " << pCallbackData->pMessage << std::endl;
                break;
            default:
                std::cerr << "\033[32m [VALIDATION LAYER] " << pCallbackData->pMessage << std::endl;
                break;
            }
            std::cerr << "\033[0m";
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
            uint32_t graphics = std::numeric_limits<uint32_t>::max();
            uint32_t presentation = std::numeric_limits<uint32_t>::max();

            inline bool isGraphicsSupported() const
            {
                return graphics != std::numeric_limits<uint32_t>::max();
            }

            inline bool isPresentationSupported() const
            {
                return presentation != std::numeric_limits<uint32_t>::max();
            }

            inline bool isGraphicsAndPresentSame() const
            {
                return graphics == presentation;
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
            uint32_t imageCount;
            vk::SurfaceTransformFlagBitsKHR transform;
            vk::PresentModeKHR presentMode;
            vk::Extent2D extent;
        };

        struct SwapchainData
        {
            vk::SwapchainKHR swapchain;
            vk::Format imageFormat;
            vk::Extent2D imageExtent;
            vector<vk::Image> images;
            vector<vk::ImageView> imageViews;

            bool destroy(const vk::Device& device)
            {
                if (swapchain)
                    device.destroySwapchainKHR(swapchain);
                if (imageViews.size() > 0)
                {
                    for (const vk::ImageView& i : imageViews)
                        device.destroyImageView(i);
                    imageViews.clear();
                    images.clear();
                }
                return true;
            }
        };

        struct CommandData
        {
            vk::CommandPool pool;
            vector<vk::CommandBuffer> buffers;

            bool destroy(const vk::Device& device)
            {
                // Freeing command buffers is optional, destorying command pool will free the buffers internally.
                if (buffers.size() > 0)
                {
                    device.freeCommandBuffers(pool, buffers);
                    buffers.clear();
                }
                if (pool)
                    device.destroyCommandPool(pool);
                return true;
            }
        };

        struct RenderData
        {
            vk::RenderPass renderPass;
            vector<vk::Framebuffer> framebuffers;

            bool destroy(const vk::Device& device)
            {
                if (renderPass)
                    device.destroyRenderPass(renderPass);
                if (framebuffers.size() > 0)
                {
                    for (const vk::Framebuffer& f : framebuffers)
                        device.destroyFramebuffer(f);
                    framebuffers.clear();
                }
                return true;
            }
        };

        struct RenderSyncData
        {
            vk::Fence renderFence;
            vk::Semaphore renderSemaphore;
            vk::Semaphore presentSemaphore;

            bool destroy(const vk::Device& device)
            {
                if (renderFence)
                {
                    VK_HANDLE_RESULT(device.waitForFences(renderFence, true, 1000000000));
                    device.destroyFence(renderFence);
                }
                if (renderSemaphore)
                    device.destroySemaphore(renderSemaphore);
                if (presentSemaphore)
                    device.destroySemaphore(presentSemaphore);
                return true;
            }
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
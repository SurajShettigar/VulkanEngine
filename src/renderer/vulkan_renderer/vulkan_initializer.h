#ifndef VULKAN_INITIALIZER_H
#define VULKAN_INITIALIZER_H

#include "vulkan_types.h"

namespace engine
{
    namespace vulkan
    {
        /**
         * @brief Creates a vulkan instance and debug messenger if necessary. Used to
         * initialize vulkan
         *
         * @param data Data necessary to create a vulkan instance and debug messenger
         * @param instance Give the Instance object which needs to be initialized
         * @param debugMessenger Give the Debug messenger object which needs to be initialized
         * @return true if initialization is sucessfull
         * @return false if initialization fails
         */
        bool createInstance(const InstanceCreateData &data,
                            vk::Instance &instance,
                            vk::DebugUtilsMessengerEXT &messenger);

        /**
         * @brief Destroys the debug messenger. Used for cleanup during application quit
         *
         * @param instance Vulkan instance object
         * @param messenger Debug messenger object
         * @return true if destruction is successfull
         * @return false if destruction fails
         */
        bool destroyDebugMessenger(vk::Instance &instance, vk::DebugUtilsMessengerEXT &messenger);

        /**
         * @brief Selects an appropriate Vulkan Physical Device (GPU). It filters
         * out GPU based on their capabilites. For example, if it a discrete GPU,
         * supports geometry shader, has required extensions etc.
         *
         * @param instance Vulkan instance object
         * @param surface Vulkan surface object
         * @param reqExtensions vector of required device extensions
         *  (Eg: VK_KHR_swapchain)
         * @return vk::PhysicalDevice if a valid GPU is found
         * @return nullptr if no valid GPU exists
         */
        vk::PhysicalDevice selectPhysicalDevice(const vk::Instance &instance,
                                                const vk::SurfaceKHR &surface,
                                                const vector<const char *> &reqExtensions = {});

        /**
         * @brief Gets the Queue Family indices based on the given Vulkan
         * Physical device and surface
         *
         * @param physicalDevice Vulkan Physical device object. (Obtained through
         * engine::vulkan::selectPhysicalDevice() function call)
         * @param surface Vulkan surface object
         * @return QueueFamilyIndices struct with necessary queue family indices.
         * The indices might not be intialized if it the physical device does not
         * support the queue families present in QueueFamilyIndices
         */
        QueueFamilyIndices getQueueFamilyIndices(const vk::PhysicalDevice &physicalDevice,
                                                 const vk::SurfaceKHR &surface);

        /**
         * @brief Get the Vulkan logical device object corresponding to the given
         * physical device object.
         *
         * @param physicalDevice Vulkan physical device object
         * @param queueFamilyIndices struct with indices of all the necessary queue families
         * @param validationLayers needed for debugging
         * @param reqExtensions The device extensions which will be used
         * (Eg: VK_KHR_swapchain)
         * @return vk::Device vulkan logical device object
         * @return nullptr if it fails to create the device
         */
        vk::Device getLogicalDevice(const vk::PhysicalDevice &physicalDevice,
                                    const QueueFamilyIndices &queueFamilyIndices,
                                    const vector<const char *> &validationLayers = {},
                                    const vector<const char *> &reqExtensions = {});

        SwapchainData createSwapchain(const vk::Device &device, const SwapchainInitData &data);
    }
}

#endif
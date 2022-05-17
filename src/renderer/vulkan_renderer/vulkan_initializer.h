#ifndef VULKAN_INITIALIZER_H
#define VULKAN_INITIALIZER_H

#include "vulkan_types.h"

namespace engine
{
    namespace vulkan
    {
        bool createInstance(const InstanceCreateData &data,
                            vk::Instance &instance,
                            vk::DebugUtilsMessengerEXT &messenger);
        bool destroyInstance(vk::Instance &instance, vk::DebugUtilsMessengerEXT &messenger);

        vk::PhysicalDevice selectPhysicalDevice(const vk::Instance &instance);
        QueueFamilyIndices getQueueFamilyIndices(const vk::PhysicalDevice &physicalDevice);
        vk::Device getLogicalDevice(const vk::PhysicalDevice &physicalDevice);
    }
}

#endif
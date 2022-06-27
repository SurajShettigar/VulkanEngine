#ifndef VULKAN_GRAPHICS_H
#define VULKAN_GRAPHICS_H

#include "vulkan_utils.h"

namespace engine
{
    namespace vulkan
    {
        RenderData getRenderData(const vk::Device& device, const SwapchainData& swapchainData);
        RenderSyncData getRenderSyncData(const vk::Device& device);
    }
}
#endif
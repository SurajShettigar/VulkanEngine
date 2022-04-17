#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include <vulkan/vulkan.hpp>

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
        static const char* ENGINE_NAME = "Vulkan";
        static const int ENINGE_VERSION[3] = {1, 0, 0};
    }
}

#endif
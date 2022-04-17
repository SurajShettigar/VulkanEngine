#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

// Vulkan header should be included before Renderer.
// This is done to ensure window header (GLFW) is included after
// vulkan is included so that GLFW knows to include vulkan functions.
#include "vulkan_renderer/vulkan_types.h"
#include "renderer.h"
#include <vector>
#include <algorithm>

using std::vector;

namespace engine
{
    namespace vulkan
    {
        class VulkanRenderer : public Renderer
        {
        private:
#if NDEBUG
            bool m_isValidationLayerEnabled = false;
#else
            bool m_isValidationLayerEnabled = true;
#endif
            const char *m_appName;
            const int *m_version;

            VkInstance m_instance;
            VkDebugUtilsMessengerEXT m_debugMessenger;
            VkPhysicalDevice m_gpu;
            VkDevice m_device;
            VkSurfaceKHR m_surface;

            std::vector<const char *> getRequiredExtenstions() const;
            bool hasRequriredExtensions(const vector<const char *> &reqExtensions) const;
            bool initVulkan();

        protected:
            bool init() override;
            void update() override;
            void render() override;
            bool clean() override;

        public:
            VulkanRenderer(Window &window, const char *appName, const int version[3])
                : Renderer(window, RendererType::Vulkan),
                  m_isValidationLayerEnabled{false},
                  m_appName{appName},
                  m_version{version} {}
            ~VulkanRenderer() = default;
        };
    }
}

#endif
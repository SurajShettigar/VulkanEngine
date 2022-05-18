#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

// Vulkan header should be included before Renderer.
// This is done to ensure window header (GLFW) is included after
// vulkan is included so that GLFW knows to include vulkan functions.
#include "vulkan_renderer/vulkan_types.h"
#include "renderer.h"

namespace engine
{
    namespace vulkan
    {
        class VulkanRenderer : public Renderer
        {
        private:
            bool m_isValidationLayerEnabled = true;
            const char *m_appName;
            const int *m_version;

            vk::Instance m_instance;
            vk::DebugUtilsMessengerEXT m_debugMessenger;
            vk::PhysicalDevice m_gpu;
            vk::Device m_device;
            vk::SurfaceKHR m_surface;

            QueueFamilyIndices m_queueFamilyIndices;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentationQueue;

            SwapchainData m_swapchainData;

            std::vector<const char *> getRequiredExtenstions() const;
            bool initSurface();
            bool initDevice();
            bool initSwapchain();
            bool initVulkan();
            bool cleanVulkan();
        protected:
            bool init() override;
            void update() override;
            void render() override;
            bool clean() override;

        public:
            VulkanRenderer(Window &window, const char *appName, const int version[3], bool enableValidationLayers = true)
                : Renderer(window, RendererType::Vulkan),
                  m_isValidationLayerEnabled{enableValidationLayers},
                  m_appName{appName},
                  m_version{version} {}
            ~VulkanRenderer() = default;
        };
    }
}

#endif
#include "vulkan_renderer.h"
#include "vulkan_renderer/vulkan_initializer.h"

vector<const char*> engine::vulkan::VulkanRenderer::getRequiredExtenstions() const
{
    // Required extensions by GLFW
    vector<const char*> reqExtensions = m_window.vkGetRequiredInstanceExtensions();
    if (m_isValidationLayerEnabled)
        reqExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return reqExtensions;
}

bool engine::vulkan::VulkanRenderer::initSurface()
{
    VkSurfaceKHR surface;
    VkResult result = m_window.vkGetWindowSurface(static_cast<VkInstance>(m_instance),
        nullptr,
        &surface);
    if (result == VK_SUCCESS)
    {
        m_surface = vk::SurfaceKHR(surface);
        return true;
    }

    return false;
}

bool engine::vulkan::VulkanRenderer::initDevice()
{
    m_gpu = selectPhysicalDevice(m_instance, m_surface, DEVICE_EXTENSIONS);
    if (m_gpu)
    {
        m_queueFamilyIndices = getQueueFamilyIndices(m_gpu, m_surface);
        m_device = getLogicalDevice(m_gpu,
            m_queueFamilyIndices,
            m_isValidationLayerEnabled
            ? VALIDATION_LAYERS
            : vector<const char*>{},
            DEVICE_EXTENSIONS);
        m_graphicsQueue = m_device.getQueue(m_queueFamilyIndices.graphics, 0);
        m_presentationQueue = m_device.getQueue(m_queueFamilyIndices.presentation, 0);
    }

    return m_gpu && m_device && m_graphicsQueue ? true : false;
}

bool engine::vulkan::VulkanRenderer::initSwapchain()
{
    return true;
}

bool engine::vulkan::VulkanRenderer::initVulkan()
{
    vk::ApplicationInfo appInfo(m_appName,
        VK_MAKE_VERSION(m_version[0], m_version[1], m_version[2]),
        ENGINE_NAME,
        VK_MAKE_VERSION(ENINGE_VERSION[0], ENINGE_VERSION[1], ENINGE_VERSION[2]),
        VK_API_VERSION_1_2);

    InstanceCreateData data{ appInfo, m_isValidationLayerEnabled, getRequiredExtenstions(), VALIDATION_LAYERS };
    bool isInstanceCreated = createInstance(data, m_instance, m_debugMessenger);

    bool isSurfaceCreated = false;
    if (isInstanceCreated)
        isSurfaceCreated = initSurface();

    bool isDeviceInit = false;
    if (isSurfaceCreated)
        isDeviceInit = initDevice();

    return isInstanceCreated && isSurfaceCreated;
}

bool engine::vulkan::VulkanRenderer::cleanVulkan()
{
    if (m_device)
        m_device.destroy();
    if (m_instance && m_surface)
        m_instance.destroySurfaceKHR(m_surface);
    if (m_isValidationLayerEnabled && m_debugMessenger)
        destroyDebugMessenger(m_instance, m_debugMessenger);
    m_instance.destroy();
    return true;
}

bool engine::vulkan::VulkanRenderer::init()
{
    bool isInit = Renderer::init();
    if (isInit)
    {
        isInit = initVulkan();
    }

    return isInit;
}

void engine::vulkan::VulkanRenderer::update()
{
    Renderer::update();
}

void engine::vulkan::VulkanRenderer::render()
{
    Renderer::render();
}

bool engine::vulkan::VulkanRenderer::clean()
{
    bool isCleaned = Renderer::clean();

    if (isCleaned)
    {
        isCleaned = cleanVulkan();
    }

    return isCleaned;
}
#include "vulkan_renderer.h"
#include "vulkan_renderer/vulkan_initializer.h"

vector<const char *> engine::vulkan::VulkanRenderer::getRequiredExtenstions() const
{
    // Required extensions by GLFW
    uint32_t reqExtensionsCount = 0;
    const char **exts = glfwGetRequiredInstanceExtensions(&reqExtensionsCount);
    vector<const char *> reqExtensions(exts, exts + reqExtensionsCount);
    if (m_isValidationLayerEnabled)
        reqExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return reqExtensions;
}

bool engine::vulkan::VulkanRenderer::initSurface()
{
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(m_instance,
                                              m_window.getGLFWWindow(),
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
    m_gpu = selectPhysicalDevice(m_instance);
    if (m_gpu)
        m_device = getLogicalDevice(m_gpu);

    return m_gpu && m_device ? true : false;
}

bool engine::vulkan::VulkanRenderer::initVulkan()
{
    vk::ApplicationInfo appInfo(m_appName,
                                VK_MAKE_VERSION(m_version[0], m_version[1], m_version[2]),
                                ENGINE_NAME,
                                VK_MAKE_VERSION(ENINGE_VERSION[0], ENINGE_VERSION[1], ENINGE_VERSION[2]),
                                VK_API_VERSION_1_2);

    InstanceCreateData data{appInfo, m_isValidationLayerEnabled, getRequiredExtenstions(), VALIDATION_LAYERS};
    bool isInstanceCreated = createInstance(data, m_instance, m_debugMessenger);

    bool isSurfaceCreated = false;
    if (isInstanceCreated)
        isSurfaceCreated = initSurface();

    return isInstanceCreated && isSurfaceCreated;
}

bool engine::vulkan::VulkanRenderer::cleanVulkan()
{
    m_instance.destroySurfaceKHR(m_surface);
    return destroyInstance(m_instance, m_debugMessenger);
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
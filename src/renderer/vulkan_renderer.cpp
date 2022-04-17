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

bool engine::vulkan::VulkanRenderer::hasRequriredExtensions(const vector<const char *> &reqExtensions) const
{
    // Search if Vulkan has all the required extensions
    vector<vk::ExtensionProperties> vExtenstions = vk::enumerateInstanceExtensionProperties();
    // For each required extension, check if it exists in the list of vulkan extensions
    auto missingExtensions = std::find_if(reqExtensions.begin(),
                                          reqExtensions.end(),
                                          [&vExtenstions](const char *reqE)
                                          {
                                              return std::find_if(vExtenstions.begin(),
                                                                  vExtenstions.end(),
                                                                  [&reqE](const vk::ExtensionProperties &e)
                                                                  {
                                                                      return strcmp(reqE, e.extensionName.data());
                                                                  }) == vExtenstions.end();
                                          });

    // If missing extensions iterator has reached the end, it found all the
    // required extensions, otherwise there are some missing extensions
    return missingExtensions == reqExtensions.end();
}

bool engine::vulkan::VulkanRenderer::initVulkan()
{
    vector<const char *> reqExtensions = getRequiredExtenstions();
    if (!hasRequriredExtensions(reqExtensions))
    {
        std::cerr << "Vulkan does not have required extensions" << std::endl;
        return false;
    }

    vk::ApplicationInfo appInfo(m_appName,
                                VK_MAKE_VERSION(m_version[0], m_version[1], m_version[2]),
                                ENGINE_NAME,
                                VK_MAKE_VERSION(ENINGE_VERSION[0], ENINGE_VERSION[1], ENINGE_VERSION[2]),
                                VK_API_VERSION_1_2);
    vk::InstanceCreateInfo createInfo({}, &appInfo, nullptr, reqExtensions);
    m_instance = vk::createInstance(createInfo);
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
    return Renderer::clean();
}
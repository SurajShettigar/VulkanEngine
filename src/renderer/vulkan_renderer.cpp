#include "vulkan_renderer.h"
#include "vulkan/vulkan_functions.h"
#include "vulkan/vulkan_graphics.h"

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
    m_swapchainData = createSwapchain(m_gpu,
        m_device,
        m_surface,
        m_queueFamilyIndices,
        m_window.getWindowResolution());

    return m_swapchainData.swapchain;
}

bool engine::vulkan::VulkanRenderer::initCommands()
{
    m_commandData = createCommandData(m_device, m_queueFamilyIndices);
    return m_commandData.pool && m_commandData.buffers.size() > 0;
}

bool engine::vulkan::VulkanRenderer::initRenderpass()
{
    m_renderData = getRenderData(m_device, m_swapchainData);

    return m_renderData.renderPass && m_renderData.framebuffers.size() > 0;
}

bool engine::vulkan::VulkanRenderer::initRenderSyncData()
{
    m_renderSyncData = getRenderSyncData(m_device);

    return m_renderSyncData.renderFence
        && m_renderSyncData.renderSemaphore
        && m_renderSyncData.presentSemaphore;
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

    bool isSwapchainInit = false;
    if (isDeviceInit)
        isSwapchainInit = initSwapchain();

    bool isCommandsInit = false;
    if (isSwapchainInit)
        isCommandsInit = initCommands();

    bool isRenderpassInit = false;
    if (isCommandsInit)
        isRenderpassInit = initRenderpass();

    bool isRenderSyncInit = false;
    if (isRenderpassInit)
        isRenderSyncInit = initRenderSyncData();

    return isInstanceCreated
        && isSurfaceCreated
        && isDeviceInit
        && isSwapchainInit
        && isCommandsInit
        && isRenderpassInit
        && isRenderSyncInit;
}

bool engine::vulkan::VulkanRenderer::cleanVulkan()
{
    if (m_device)
    {
        m_renderSyncData.destroy(m_device);
        m_renderData.destroy(m_device);
        m_commandData.destroy(m_device);
        m_swapchainData.destroy(m_device);

        m_device.destroy();
    }

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

    VK_HANDLE_RESULT(m_device.waitForFences(m_renderSyncData.renderFence, true, 1000000000));
    m_device.resetFences(m_renderSyncData.renderFence);

    uint32_t imgIndex = m_device.acquireNextImageKHR(m_swapchainData.swapchain, 1000000000, m_renderSyncData.presentSemaphore).value;

    m_commandData.buffers[0].reset();
    m_commandData.buffers[0].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    vk::ClearValue clearValue;
    float flash = abs(sin(m_currentFrameNumber / 120.f));
    std::array<float, 4> color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    color[2] = flash;
    clearValue.setColor(vk::ClearColorValue(color));

    m_commandData.buffers[0].beginRenderPass(vk::RenderPassBeginInfo(m_renderData.renderPass,
        m_renderData.framebuffers[imgIndex],
        vk::Rect2D({ 0, 0 }, { m_swapchainData.imageExtent }),
        clearValue), vk::SubpassContents::eInline);

    m_commandData.buffers[0].endRenderPass();

    m_commandData.buffers[0].end();

    vk::PipelineStageFlags pipelineFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    m_graphicsQueue.submit(vk::SubmitInfo(m_renderSyncData.presentSemaphore,
        pipelineFlags,
        m_commandData.buffers[0],
        m_renderSyncData.renderSemaphore), m_renderSyncData.renderFence);

    VK_HANDLE_RESULT(m_presentationQueue.presentKHR(vk::PresentInfoKHR(m_renderSyncData.renderSemaphore,
        m_swapchainData.swapchain,
        imgIndex)));

    m_currentFrameNumber++;
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
#include <vulkan/vulkan.hpp>
#include "window.h"

void engine::Window::setWindowCloseListener(std::function<void()> callback)
{
    m_onWindowClosed = callback;
}

void engine::Window::setWindowType(WindowType type)
{
    m_type = type;
}

// Utility functions
std::array<int, 2> engine::Window::getWindowResolution() const
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_current, &width, &height);
    return std::array<int, 2>{width, height};
}

// Vulkan API related functions
std::vector<const char*> engine::Window::vkGetRequiredInstanceExtensions() const
{
    if (m_type != WindowType::Vulkan)
    {
        std::cerr << "Window Error: Cannot call Vulkan functions with current Renderer" << std::endl;
        return {};
    }

    uint32_t count = 0;
    const char** exts = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> reqExtensions(exts, exts + count);
    return reqExtensions;
}

VkResult engine::Window::vkGetWindowSurface(VkInstance instance,
    const VkAllocationCallbacks* allocator,
    VkSurfaceKHR* surface) const
{
    if (m_type != WindowType::Vulkan)
    {
        std::cerr << "Window Error: Cannot call Vulkan functions with current Renderer" << std::endl;
        return {};
    }

    return glfwCreateWindowSurface(instance, m_current, allocator, surface);
}

bool engine::Window::init()
{
    if (m_type == WindowType::None)
    {
        cerr << "Window.init error: Window type not set" << endl;
        return false;
    }

    if (!glfwInit())
    {
        cerr << "Window.init error: Failed to initialize glfw" << endl;
        return false;
    }

    switch (m_type)
    {
    case WindowType::OpenGL:
        break;
    case WindowType::Vulkan:
        if (!glfwVulkanSupported())
        {
            cerr << "Current platform does not support Vulkan" << endl;
            return false;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    default:
        break;
    }

    m_current = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

    if (!m_current)
    {
        clean();
        cerr << "Window.init error: Failed to create glfw window" << endl;
        return false;
    }

    return true;
}

void engine::Window::update()
{
    if (glfwWindowShouldClose(m_current))
    {
        m_onWindowClosed();
        return;
    }
    glfwPollEvents();
}

bool engine::Window::clean()
{
    m_onWindowClosed = nullptr;
    glfwTerminate();
    return true;
}
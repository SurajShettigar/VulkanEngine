#include "window.h"

void engine::Window::setWindowCloseListener(std::function<void()> callback)
{
    m_windowCloseListener = callback;
}

void engine::Window::setWindowType(WindowType type)
{
    m_type = type;
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
        m_windowCloseListener();
        return;
    }
    glfwPollEvents();
}

bool engine::Window::clean()
{
    m_windowCloseListener = nullptr;
    glfwTerminate();
    return true;
}
#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <functional>
#include <array>
#include <vector>

#include <GLFW/glfw3.h>

// Forward declaration of Vulkan API
enum VkResult;
struct VkInstance_T;
typedef VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T* VkSurfaceKHR;

namespace engine
{
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;

    enum class WindowType
    {
        None,
        OpenGL,
        Vulkan
    };

    class Window
    {
    private:
        WindowType m_type = WindowType::None;
        GLFWwindow* m_current;

        std::function<void()> m_onWindowClosed;

    public:
        string name = "Window";
        int width = 1280;
        int height = 720;

        Window(string name = "Window", int width = 1280, int height = 720)
            : m_type{ WindowType::None }, name{ name }, width{ width }, height{ height } {}

        Window(const Window& window)
        {
            m_current = window.m_current;
            m_onWindowClosed = window.m_onWindowClosed;
            name = window.name;
            width = window.height;
            height = window.height;
        }

        Window& operator=(const Window& window)
        {
            m_current = window.m_current;
            m_onWindowClosed = window.m_onWindowClosed;
            name = window.name;
            width = window.height;
            height = window.height;
            return *this;
        }

        // GLFWwindow* const getGLFWWindow() const
        // {
        //     return m_current;
        // }

        void setWindowCloseListener(std::function<void()> callback);
        void setWindowType(WindowType type);

        // Utility functions
        std::array<int, 2> getWindowResolution() const;

        // Vulkan API related functions
        std::vector<const char*> vkGetRequiredInstanceExtensions() const;

        VkResult vkGetWindowSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const;

        bool init();
        void update();
        bool clean();
    };
}

#endif
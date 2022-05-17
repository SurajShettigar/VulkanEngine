#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <functional>

#include <GLFW/glfw3.h>

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
        GLFWwindow *m_current;

        std::function<void()> m_windowCloseListener;

    public:
        string name = "Window";
        int width = 1280;
        int height = 720;

        Window(string name = "Window", int width = 1280, int height = 720)
            : m_type{WindowType::None}, name{name}, width{width}, height{height} {}

        Window(const Window &window)
        {
            m_current = window.m_current;
            m_windowCloseListener = window.m_windowCloseListener;
            name = window.name;
            width = window.height;
            height = window.height;
        }
        Window &operator=(const Window &window)
        {
            m_current = window.m_current;
            m_windowCloseListener = window.m_windowCloseListener;
            name = window.name;
            width = window.height;
            height = window.height;
            return *this;
        }

        GLFWwindow *const getGLFWWindow() const
        {
            return m_current;
        }

        void setWindowCloseListener(std::function<void()> callback);
        void setWindowType(WindowType type);

        bool init();
        void update();
        bool clean();
    };
}

#endif
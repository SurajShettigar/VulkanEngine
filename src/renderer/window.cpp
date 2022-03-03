#include "window.h"

void Window::setWindowCloseListener(void (*callback)())
{
   m_windowCloseListener = callback; 
}

bool Window::init()
{
    if(!glfwInit())
    {
        cerr << "Window.init error: Failed to initialize glfw" << endl;
        return false;
    }
    
    m_current = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    
    if(!m_current)
    {
        clean();
        cerr << "Window.init error: Failed to create glfw window" << endl;
        return false;
    }

    return true;
}

void Window::update()
{
    if(glfwWindowShouldClose(m_current))
    {
        m_windowCloseListener();
        return;
    }
    glfwPollEvents();
}

bool Window::clean()
{
    m_windowCloseListener = nullptr;
    glfwTerminate();
    return true;
}
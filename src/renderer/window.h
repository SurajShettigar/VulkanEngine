#include<iostream>
#include<string>

#include<GLFW/glfw3.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

class Window
{
private:
    GLFWwindow* m_current;

    void (*m_windowCloseListener)();
public:
    string name = "Vulkan Window";
    int width = 1280;
    int height = 720;

    const GLFWwindow* const current = m_current;

    Window() = default;
    Window(string name = "Vulkan Window", int width = 1280, int height = 720)
    : name{name}, width{width}, height{height} {}

    void setWindowCloseListener(void (*callback)());

    bool init();
    void update();
    bool clean();
};
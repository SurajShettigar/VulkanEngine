#include "renderer.h"

bool engine::Renderer::init()
{
    return m_window.init();
}

void engine::Renderer::update()
{
    m_window.update();
}

void engine::Renderer::render()
{
}

bool engine::Renderer::clean()
{
    return m_window.clean();
}

bool engine::Renderer::run()
{
    bool isInit = init();

    if (!isInit)
    {
        cerr << "Failed to initialize Renderer" << endl;
        return false;
    }

    m_isRunning = true;

    while (m_isRunning)
    {
        update();
        render();
    }

    m_isRunning = false;

    bool isCleaned = clean();

    if (!isCleaned)
    {
        cerr << "Failed to clean Renderer" << endl;
        return false;
    }

    return true;
}

engine::Renderer::Renderer(Window &window, RendererType type)
    : m_type{type},
      m_isRunning{false},
      m_currentFrameNumber{0},
      m_window{window}
{
    m_window.setWindowCloseListener([=]()
                                    { m_isRunning = false; });

    switch (type)
    {
    case RendererType::OpenGL:
        m_window.setWindowType(WindowType::OpenGL);
        break;
    case RendererType::Vulkan:
        m_window.setWindowType(WindowType::Vulkan);
        break;
    default:
        m_window.setWindowType(WindowType::None);
        break;
    }
}

engine::Renderer::~Renderer()
{
    clean();
}
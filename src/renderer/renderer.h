#ifndef RENDERER_H
#define RENDERER_H

#include "renderer_types.h"
#include "window.h"

namespace engine
{
    class Renderer
    {
    protected:
        RendererType m_type = RendererType::None;
        bool m_isRunning = false;
        uint32_t m_currentFrameNumber = 0;

        Window &m_window;

        virtual bool init();
        virtual void update();
        virtual void render();
        virtual bool clean();

    public:
        Renderer(Window &window, RendererType type = RendererType::None);
        virtual ~Renderer();

        bool run();
    };
}

#endif
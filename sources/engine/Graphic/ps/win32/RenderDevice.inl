#include "Graphic/OpenGL.h"

namespace Graphic
{
    class _GraphicExport RenderDevice : public IRenderDevice
    {
    public:
        RenderDevice();
        ~RenderDevice();

        Bool                    Initialize( HINSTANCE hInstance, HWND window, Bool debug = false );
        void                    Destroy();
        void                    Swap();

    private:
        HWND                    m_window;
        HDC                     m_deviceContext;
        HGLRC                   m_renderContext;
    };
}

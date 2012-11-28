#include "Graphic/RenderDevice.h"

#include "Core/Assert.h"

namespace Graphic
{
    RenderDevice::RenderDevice()
    : m_window( 0 )
    , m_deviceContext( 0 )
    , m_renderContext( 0 )
    {

    }

    RenderDevice::~RenderDevice()
    {
        CARBON_ASSERT( m_window == 0 );
        CARBON_ASSERT( m_deviceContext == 0 );
        CARBON_ASSERT( m_renderContext == 0 );
    }

    Bool RenderDevice::Initialize( HWND window )
    {
        CARBON_ASSERT( m_window == 0 );
        CARBON_ASSERT( m_deviceContext == 0 );
        CARBON_ASSERT( m_renderContext == 0 );

        m_window = window;

        m_deviceContext = GetDC( m_window );
        if ( !m_deviceContext )
        {
            m_window = 0;
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
            PFD_TYPE_RGBA,                                              //The kind of framebuffer. RGBA or palette.
            24,                                                         //Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                                                         //Number of bits for the depthbuffer
            8,                                                          //Number of bits for the stencilbuffer
            0,                                                          //Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        U32 pixelFormat = ChoosePixelFormat( m_deviceContext, &pfd );
        if ( !pixelFormat )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        if ( !SetPixelFormat( m_deviceContext, pixelFormat, &pfd ) )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB   , 4,
            WGL_CONTEXT_MINOR_VERSION_ARB   , 2,
            WGL_CONTEXT_PROFILE_MASK_ARB    , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0 //End
        };

        m_renderContext = OpenGL::CreateContext( m_deviceContext, attribs );
        if ( !m_renderContext )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        return true;
    }

    void RenderDevice::Destroy()
    {
        if ( m_renderContext )
        {
            OpenGL::DestroyContext( m_renderContext );
            m_renderContext = 0;
        }

        if ( m_deviceContext )
        {
            ReleaseDC( m_window, m_deviceContext );
            m_deviceContext = 0;
        }

        m_window = 0;
    }

    void RenderDevice::Swap()
    {
        SwapBuffers(m_deviceContext);
    }
}

#include "Graphic/RenderDevice.h"

#include "Core/Assert.h"

namespace Graphic
{
    S32 CreatePixelFormat( HINSTANCE hInstance, const int * attribs )
    {
        HWND hwnd;
        WNDCLASS wc;

        wc.style = 0;
        wc.lpfnWndProc = DefWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
        wc.lpszMenuName =  NULL;
        wc.lpszClassName = "CreatePixelFormatWndClass";

        if ( !RegisterClass(&wc) ) return FALSE;

        hwnd = CreateWindow( "CreatePixelFormatWndClass"
                        ,"FAKE"
                        ,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                        ,CW_USEDEFAULT
                        ,CW_USEDEFAULT
                        ,1280
                        ,720
                        ,NULL
                        ,NULL
                        ,hInstance
                        ,NULL );

        if ( !hwnd ) return FALSE;

        HDC deviceContext = GetDC( hwnd );

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

        S32 pixelFormat = ChoosePixelFormat( deviceContext, &pfd );
        if ( !pixelFormat )
        {
            DestroyWindow( hwnd );
            UnregisterClass( "CreatePixelFormatWndClass", hInstance );
            return 0;
        }

        if ( !SetPixelFormat( deviceContext, pixelFormat, &pfd ) )
        {
            DestroyWindow( hwnd );
            UnregisterClass( "CreatePixelFormatWndClass", hInstance );
            return 0;
        }

        pixelFormat = OpenGL::CreatePixelFormat( deviceContext, attribs );

        DestroyWindow( hwnd );
        UnregisterClass( "CreatePixelFormatWndClass", hInstance );

        return pixelFormat;
    }

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

    Bool RenderDevice::Initialize( HINSTANCE hInstance, HWND window, Bool debug )
    {
        CARBON_ASSERT( m_window == 0 );
        CARBON_ASSERT( m_deviceContext == 0 );
        CARBON_ASSERT( m_renderContext == 0 );

        m_window = window;

        int pfAttribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB          , GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB          , GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB           , GL_TRUE,
            WGL_PIXEL_TYPE_ARB              , WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB              , 24,
            WGL_DEPTH_BITS_ARB              , 24,
            WGL_STENCIL_BITS_ARB            , 8,
            WGL_SAMPLE_BUFFERS_ARB          , 1,
            WGL_SAMPLES_ARB                 , 4, // MSAA x4
            0 //End
        };

        S32 pixelFormat = CreatePixelFormat( hInstance, pfAttribs );
        if ( !pixelFormat )
        {
            m_window = 0;
            return false;
        }

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

        if ( !SetPixelFormat( m_deviceContext, pixelFormat, &pfd ) )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        int flags = debug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0;

        int rcAttribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB   , 4,
            WGL_CONTEXT_MINOR_VERSION_ARB   , 2,
            WGL_CONTEXT_FLAGS_ARB           , flags,
            WGL_CONTEXT_PROFILE_MASK_ARB    , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0 //End
        };

        m_renderContext = OpenGL::CreateContext( m_deviceContext, rcAttribs );
        if ( !m_renderContext )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        ClearCache();

        return true;
    }

    void RenderDevice::Destroy()
    {
        ClearCache();

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

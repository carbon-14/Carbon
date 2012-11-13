#include "Graphic/OpenGL.h"

META_GL_FUNCTIONS( DEFINE_GL_FUNCTIONS );

namespace Graphic
{
    HGLRC OpenGL::CreateRenderContext( HDC hDC, HGLRC hShareContext, const int *attribList )
    {
        HGLRC dummyContext = wglCreateContext( hDC );
        if ( !dummyContext )
        {
            return 0;
        }

        if ( !wglMakeCurrent( hDC, dummyContext ) )
        {
            wglDeleteContext( dummyContext );
            return 0;
        }

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress( "wglCreateContextAttribsARB" );

        if ( !wglCreateContextAttribsARB )
        {
            return 0;
        }

        HGLRC renderContext = wglCreateContextAttribsARB( hDC, hShareContext, attribList );

        wglMakeCurrent( 0, 0 );
        wglDeleteContext( dummyContext );

        if ( !renderContext )
        {
            return 0;
        }

        wglMakeCurrent( hDC, renderContext );

        return renderContext;
    }

    void OpenGL::DestroyRenderContext( HGLRC renderContext )
    {
        wglMakeCurrent( 0, 0 );
        wglDeleteContext( renderContext );
    }

    void OpenGL::LoadFunctions()
    {
        META_GL_FUNCTIONS( LOAD_GL_FUNCTIONS );
    }
}

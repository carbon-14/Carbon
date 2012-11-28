#include "Graphic/OpenGL.h"

META_GL_FUNCTIONS( DEFINE_GL_FUNCTION );

namespace Graphic
{
    HGLRC OpenGL::CreateContext( HDC hDC, const int *attribList )
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
            wglDeleteContext( dummyContext );
            return 0;
        }

        HGLRC context = wglCreateContextAttribsARB( hDC, 0, attribList );

        wglMakeCurrent( 0, 0 );
        wglDeleteContext( dummyContext );

        if ( !context )
        {
            return 0;
        }

        wglMakeCurrent( hDC, context );

        META_GL_FUNCTIONS( LOAD_GL_FUNCTION );

        return context;
    }

    void OpenGL::DestroyContext( HGLRC context )
    {
        META_GL_FUNCTIONS( UNLOAD_GL_FUNCTION );

        wglMakeCurrent( 0, 0 );
        wglDeleteContext( context );
    }
}

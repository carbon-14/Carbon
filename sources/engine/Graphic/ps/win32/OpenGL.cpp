#include "Graphic/OpenGL.h"

// GL_VERSION_1_5
PFNGLBINDBUFFERPROC                     glBindBuffer                = 0;
PFNGLDELETEBUFFERSPROC                  glDeleteBuffers             = 0;
PFNGLGENBUFFERSPROC                     glGenBuffers                = 0;
PFNGLBUFFERDATAPROC                     glBufferData                = 0;
PFNGLBUFFERSUBDATAPROC                  glBufferSubData             = 0;

// GL_VERSION_2_0
PFNGLATTACHSHADERPROC                   glAttachShader              = 0;
PFNGLCOMPILESHADERPROC                  glCompileShader             = 0;
PFNGLCREATEPROGRAMPROC                  glCreateProgram             = 0;
PFNGLCREATESHADERPROC                   glCreateShader              = 0;
PFNGLDELETEPROGRAMPROC                  glDeleteProgram             = 0;
PFNGLDELETESHADERPROC                   glDeleteShader              = 0;
PFNGLDETACHSHADERPROC                   glDetachShader              = 0;
PFNGLDISABLEVERTEXATTRIBARRAYPROC       glDisableVertexAttribArray  = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC        glEnableVertexAttribArray   = 0;
PFNGLGETPROGRAMIVPROC                   glGetProgramiv              = 0;
PFNGLGETPROGRAMINFOLOGPROC              glGetProgramInfoLog         = 0;
PFNGLGETSHADERIVPROC                    glGetShaderiv               = 0;
PFNGLGETSHADERINFOLOGPROC               glGetShaderInfoLog          = 0;
PFNGLLINKPROGRAMPROC                    glLinkProgram               = 0;
PFNGLSHADERSOURCEPROC                   glShaderSource              = 0;
PFNGLUSEPROGRAMPROC                     glUseProgram                = 0;
PFNGLVERTEXATTRIBPOINTERPROC            glVertexAttribPointer       = 0;

// GL_ARB_vertex_array_object
PFNGLBINDVERTEXARRAYPROC                glBindVertexArray           = 0;
PFNGLDELETEVERTEXARRAYSPROC             glDeleteVertexArrays        = 0;
PFNGLGENVERTEXARRAYSPROC                glGenVertexArrays           = 0;
PFNGLISVERTEXARRAYPROC                  glIsVertexArray             = 0;

// GL_ARB_get_program_binary
PFNGLGETPROGRAMBINARYPROC               glGetProgramBinary          = 0;
PFNGLPROGRAMBINARYPROC                  glProgramBinary             = 0;
PFNGLPROGRAMPARAMETERIPROC              glProgramParameteri         = 0;


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
        //	GL_VERSION_1_5
        glBindBuffer                = (PFNGLBINDBUFFERPROC)                 wglGetProcAddress("glBindBuffer"                );
        glDeleteBuffers             = (PFNGLDELETEBUFFERSPROC)              wglGetProcAddress("glDeleteBuffers"             );
        glGenBuffers                = (PFNGLGENBUFFERSPROC)                 wglGetProcAddress("glGenBuffers"                );
        glBufferData                = (PFNGLBUFFERDATAPROC)                 wglGetProcAddress("glBufferData"                );
        glBufferSubData             = (PFNGLBUFFERSUBDATAPROC)              wglGetProcAddress("glBufferSubData"             );

        //	GL_VERSION_2_0
        glAttachShader              = (PFNGLATTACHSHADERPROC)               wglGetProcAddress("glAttachShader"              );
        glCompileShader             = (PFNGLCOMPILESHADERPROC)              wglGetProcAddress("glCompileShader"             );
        glCreateProgram             = (PFNGLCREATEPROGRAMPROC)              wglGetProcAddress("glCreateProgram"             );
        glCreateShader              = (PFNGLCREATESHADERPROC)               wglGetProcAddress("glCreateShader"              );
        glDeleteProgram             = (PFNGLDELETEPROGRAMPROC)              wglGetProcAddress("glDeleteProgram"             );
        glDeleteShader              = (PFNGLDELETESHADERPROC)               wglGetProcAddress("glDeleteShader"              );
        glDetachShader              = (PFNGLDETACHSHADERPROC)               wglGetProcAddress("glDetachShader"              );
        glDisableVertexAttribArray  = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)   wglGetProcAddress("glDisableVertexAttribArray"  );
        glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)    wglGetProcAddress("glEnableVertexAttribArray"   );
        glGetProgramiv              = (PFNGLGETPROGRAMIVPROC)               wglGetProcAddress("glGetProgramiv"              );
        glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)          wglGetProcAddress("glGetProgramInfoLog"         );
        glGetShaderiv               = (PFNGLGETSHADERIVPROC)                wglGetProcAddress("glGetShaderiv"               );
        glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)           wglGetProcAddress("glGetShaderInfoLog"          );
        glLinkProgram               = (PFNGLLINKPROGRAMPROC)                wglGetProcAddress("glLinkProgram"               );
        glShaderSource              = (PFNGLSHADERSOURCEPROC)               wglGetProcAddress("glShaderSource"              );
        glUseProgram                = (PFNGLUSEPROGRAMPROC)                 wglGetProcAddress("glUseProgram"                );
        glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)        wglGetProcAddress("glVertexAttribPointer"       );

        // GL_ARB_vertex_array_object
        glBindVertexArray           = (PFNGLBINDVERTEXARRAYPROC)            wglGetProcAddress("glBindVertexArray"           );
        glDeleteVertexArrays        = (PFNGLDELETEVERTEXARRAYSPROC)         wglGetProcAddress("glDeleteVertexArrays"        );
        glGenVertexArrays           = (PFNGLGENVERTEXARRAYSPROC)            wglGetProcAddress("glGenVertexArrays"           );
        glIsVertexArray             = (PFNGLISVERTEXARRAYPROC)              wglGetProcAddress("glIsVertexArray"             );

        // GL_ARB_get_program_binary
        glGetProgramBinary          = (PFNGLGETPROGRAMBINARYPROC)           wglGetProcAddress("glGetProgramBinary"          );
        glProgramBinary             = (PFNGLPROGRAMBINARYPROC)              wglGetProcAddress("glProgramBinary"             );
        glProgramParameteri         = (PFNGLPROGRAMPARAMETERIPROC)          wglGetProcAddress("glProgramParameteri"         );
    }
}

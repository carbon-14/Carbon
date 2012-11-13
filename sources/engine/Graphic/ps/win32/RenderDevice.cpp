#include "Graphic/RenderDevice.h"

#include "Core/MemoryManager.h"

#include "Core/Assert.h"
#include "Core/Trace.h"
#include "Core/String.h"

namespace Graphic
{
    const S32 ToGLShaderType[] =
    {
        GL_VERTEX_SHADER,   // ST_VERTEX_SHADER
        GL_FRAGMENT_SHADER, // ST_FRAGMENT_SHADER
        GL_GEOMETRY_SHADER  // ST_GEOMETRY_SHADER
    };

    const GLenum ToGLVertexArrayUsage[] =
    {
        GL_STATIC_DRAW,     // VBU_STATIC
        GL_DYNAMIC_DRAW,    // VBU_DYNAMIC
        GL_STREAM_DRAW      // VBU_STREAM
    };

    const GLenum ToGLDataType[] =
    {
        GL_BYTE,            // DT_S8
        GL_UNSIGNED_BYTE,   // DT_U8
        GL_SHORT,           // DT_S16
        GL_UNSIGNED_SHORT,  // DT_U16
        GL_INT,             // DT_S32
        GL_UNSIGNED_INT,    // DT_U32
        GL_FLOAT,           // DT_F32
        GL_DOUBLE           // DT_F64
    };

    const GLenum ToGLPrimitiveType[] =
    {
        GL_POINTS,      // PT_POINTS
        GL_LINES,       // PT_LINES
        GL_TRIANGLES    // PT_TRIANGLES
    };

    const GLuint ToVertexSemanticMap[] =
    {
        1 << VS_POSITION,
        1 << VS_NORMAL,
        1 << VS_TANGENT,
        1 << VS_BINORMAL,
        1 << VS_COLOR,
        1 << VS_TEXCOORD0,
        1 << VS_TEXCOORD1
    };

    struct VertexArray
    {
        GLuint  m_va;
        GLuint  m_vb;
        GLuint  m_ib;
        GLuint  m_attribMap;
        GLsizei m_vertexCount;
        GLenum  m_indexType;
        GLsizei m_indexCount;
    };

    //===================================================================================

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

        HGLRC dummyContext = wglCreateContext( m_deviceContext );
        if ( !dummyContext )
        {
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        if ( !wglMakeCurrent( m_deviceContext, dummyContext ) )
        {
            wglDeleteContext( dummyContext );
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress( "wglCreateContextAttribsARB" );

        if ( !wglCreateContextAttribsARB )
        {
            return 0;
        }

        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB   , 4,
            WGL_CONTEXT_MINOR_VERSION_ARB   , 2,
            WGL_CONTEXT_PROFILE_MASK_ARB    , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0 //End
        };

        m_renderContext = wglCreateContextAttribsARB( m_deviceContext, 0, attribs );
        if ( !m_renderContext )
        {
            if ( dummyContext )
            {
                wglMakeCurrent( 0, 0 );
                wglDeleteContext( dummyContext );
            }
            m_deviceContext = 0;
            m_window = 0;
            return false;
        }

        wglMakeCurrent( 0, 0 );
        wglDeleteContext( dummyContext );
        wglMakeCurrent( m_deviceContext, m_renderContext );

        OpenGL::LoadFunctions();

        return true;
    }

    void RenderDevice::Destroy()
    {
        if ( m_renderContext )
        {
            wglMakeCurrent( 0, 0 );
            wglDeleteContext( m_renderContext );
            m_renderContext = 0;
        }
        if ( m_deviceContext )
        {
            ReleaseDC( m_window, m_deviceContext );
            m_deviceContext = 0;
        }
        m_window = 0;
    }

    VertexArray * RenderDevice::CreateVertexArray(
        const AttribDeclaration attribDecl[],
        SizeT attribCount,
        SizeT vertexSize,
        SizeT vertexCount,
        const void * vertexData,
        DataType indexType,
        SizeT indexCount,
        const void * indexData,
        VertexArrayUsage usage )
    {
        VertexArray * v = (VertexArray*)Core::UnknownAllocator::Allocate( sizeof( VertexArray ) );

        v->m_attribMap = 0;

        glGenVertexArrays( 1, &v->m_va );
        glBindVertexArray( v->m_va );

        glGenBuffers( 1, &v->m_vb );
        glBindBuffer( GL_ARRAY_BUFFER, v->m_vb );
        glBufferData( GL_ARRAY_BUFFER, vertexCount * vertexSize, vertexData, ToGLVertexArrayUsage[ usage ] );
        for ( SizeT i=0; i<attribCount; ++i )
        {
            const AttribDeclaration& attrib = attribDecl[ i ];
            v->m_attribMap |= ToVertexSemanticMap[ attrib.m_semantic ];
            glEnableVertexAttribArray( attrib.m_semantic );
            glVertexAttribPointer( attrib.m_semantic, attrib.m_size, ToGLDataType[ attrib.m_type ], attrib.m_normalized, vertexSize, reinterpret_cast< const void * >( attrib.m_offset ) );
            glDisableVertexAttribArray( attrib.m_semantic );
        }
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        if ( indexData )
        {
            glGenBuffers( 1, &v->m_ib );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, v->m_ib );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexCount * DataTypeSize[ indexType ], indexData, ToGLVertexArrayUsage[ usage ] );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

            v->m_indexType = ToGLDataType[ indexType ];
            v->m_indexCount = indexCount;
        }

        glBindVertexArray( 0 );

        return v;
    }

    void RenderDevice::DestroyVertexArray( VertexArray * v )
    {
        CARBON_ASSERT( v );

        glDeleteVertexArrays( 1, &v->m_va );
        glDeleteBuffers( 1, &v->m_vb );
        glDeleteBuffers( 1, &v->m_ib );

        Core::UnknownAllocator::Deallocate( v );
    }

    U32 RenderDevice::CreateProgram( const Char * srcBuffers[], SizeT srcSizes[], ShaderType srcTypes[], SizeT count )
    {
        CARBON_ASSERT( count > 0 );

        GLuint program = glCreateProgram();

        for ( SizeT i=0; i<count; ++i )
        {
            CARBON_ASSERT( srcSizes[ i ] > 0 );

            GLuint shader = glCreateShader( ToGLShaderType[ srcTypes[ i ] ] );
            glShaderSource( shader, 1, &srcBuffers[ i ], NULL );
            glCompileShader( shader );

            GLint compileStatus = GL_FALSE;
		    glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

            if ( compileStatus == GL_FALSE )
            {
                GLint logLength;
                GLchar buffer[ 1024 ];

                glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
                if ( logLength > 1024 )
                {
                    logLength = 1024;
                }

                glGetShaderInfoLog( shader, logLength, NULL, buffer );
                buffer[ logLength ] = '\n';
			    CARBON_TRACE( buffer );

                return 0;
            }

            glAttachShader( program, shader );
            glDeleteShader( shader );
        }

        glLinkProgram( program );

        if ( ! program )
        {
            return 0;
        }

		GLint linkStatus = GL_FALSE;
		glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );

        if ( linkStatus == GL_FALSE )
        {
            GLint logLength;
            GLchar buffer[ 1024 ];

            glGetShaderiv( program, GL_INFO_LOG_LENGTH, &logLength );
            if ( logLength > 1024 )
            {
                logLength = 1024;
            }

            glGetProgramInfoLog( program, logLength, NULL, buffer );
            buffer[ logLength ] = '\n';
            CARBON_TRACE( buffer );

            return 0;
        }

        return program;
    }

    void RenderDevice::DeleteProgram( U32 program )
    {
        glDeleteProgram( program );
    }

    void RenderDevice::GetProgramBinary( U32 program, void *& binary, SizeT& size )
    {
        GLint binLength;
        glGetProgramiv( program, GL_PROGRAM_BINARY_LENGTH, &binLength );

        size = binLength + sizeof(GLenum);

        binary          = Core::UnknownAllocator::Allocate( size );
        GLenum * fmt    = (GLenum*)binary;
        void * buffer   = fmt + 1;

        glGetProgramBinary( program, binLength, NULL, fmt, buffer );
    }

    U32 RenderDevice::CreateProgramBinary( const void * binary, SizeT size )
    {
        const GLenum * fmt  = (GLenum*)binary;
        const void * buffer = fmt + 1;
        SizeT binLength     = size - sizeof(GLenum);

        GLuint program = glCreateProgram();
        glProgramBinary( program, *fmt, buffer, binLength );

        GLint linkStatus = GL_FALSE;
		glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );
		
		if ( linkStatus == GL_FALSE )
		{
            GLint logLength;
            GLchar buffer[ 1024 ];

            glGetShaderiv( program, GL_INFO_LOG_LENGTH, &logLength );
            if ( logLength > 1024 )
            {
                logLength = 1024;
            }

            glGetProgramInfoLog( program, logLength, NULL, buffer );
            buffer[ logLength ] = '\n';

            CARBON_TRACE( buffer );

            return 0;
		}

        return program;
    }

    void RenderDevice::UseProgram( Handle program )
    {
        glUseProgram( (GLuint)program );
    }

    void RenderDevice::Draw( PrimitiveType primitive, VertexArray * va )
    {
        CARBON_ASSERT( va );

        glBindVertexArray( va->m_va );

        GLuint attribIdxCount = 0;
        GLuint attribIdx[VS_COUNT];
        for ( SizeT i=0; i<VS_COUNT; ++i )
        {
            attribIdx[attribIdxCount]   = i;
            attribIdxCount              += ( va->m_attribMap & ToVertexSemanticMap[i] ) >> i;
        }

        for ( SizeT i=0; i<attribIdxCount; ++i )
        {
            glEnableVertexAttribArray( attribIdx[i] );
        }

        if ( va->m_ib )
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, va->m_ib );
            glDrawElements( ToGLPrimitiveType[ primitive ], va->m_indexCount, va->m_indexType, (GLvoid*)0 );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        }
        else
        {
            glDrawArrays( ToGLPrimitiveType[ primitive ], 0, va->m_vertexCount );
        }

        for ( SizeT i=0; i<attribIdxCount; ++i )
        {
            glDisableVertexAttribArray( attribIdx[i] );
        }

        glBindVertexArray( 0 );
    }

    void RenderDevice::SetViewport( U32 x, U32 y, U32 w, U32 h )
    {
        glViewport( x, y, w, h );
    }

    void RenderDevice::ClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        glClearColor( r, g, b, a );
        glClear( GL_COLOR_BUFFER_BIT );
    }

    void RenderDevice::Swap()
    {
        SwapBuffers(m_deviceContext);
    }
}
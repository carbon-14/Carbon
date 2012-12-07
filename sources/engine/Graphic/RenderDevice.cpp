#include "Graphic/RenderDevice.h"

#include "Core/MemoryManager.h"

#include "Core/Assert.h"
#include "Core/Trace.h"

namespace Graphic
{
    const S32 ToGLShaderType[] =
    {
        GL_VERTEX_SHADER,   // ST_VERTEX_SHADER
        GL_FRAGMENT_SHADER, // ST_FRAGMENT_SHADER
        GL_GEOMETRY_SHADER  // ST_GEOMETRY_SHADER
    };

    const GLenum ToGLBufferUsage[] =
    {
        GL_STATIC_DRAW,     // BU_STATIC
        GL_DYNAMIC_DRAW,    // BU_DYNAMIC
        GL_STREAM_DRAW      // BU_STREAM
    };

    const GLenum ToGLDataType[] =
    {
        GL_BYTE,                        // DT_S8
        GL_UNSIGNED_BYTE,               // DT_U8
        GL_SHORT,                       // DT_S16
        GL_UNSIGNED_SHORT,              // DT_U16
        GL_HALF_FLOAT,                  // DT_F16
        GL_INT,                         // DT_S32
        GL_UNSIGNED_INT,                // DT_U32
        GL_FLOAT,                       // DT_F32
        GL_DOUBLE,                      // DT_F64
        GL_INT_2_10_10_10_REV,          // DT_S2_10_10_10
        GL_UNSIGNED_INT_2_10_10_10_REV  // DT_U2_10_10_10
    };

    const GLenum ToGLPrimitiveType[] =
    {
        GL_POINTS,      // PT_POINTS
        GL_LINES,       // PT_LINES
        GL_TRIANGLES    // PT_TRIANGLES
    };

    const GLenum ToGLMinFilterType[3][2] =  // [ mip ] [ min ]
    {
        { GL_NEAREST                , GL_LINEAR                 },  // min : FT_POINT mip FT_NONE   , min : FT_LINEAR mip FT_NONE
        { GL_NEAREST_MIPMAP_NEAREST , GL_LINEAR_MIPMAP_NEAREST  },  // min : FT_POINT mip FT_POINT  , min : FT_LINEAR mip FT_POINT
        { GL_NEAREST_MIPMAP_LINEAR  , GL_LINEAR_MIPMAP_LINEAR   }   // min : FT_POINT mip FT_LINEAR , min : FT_LINEAR mip FT_LINEAR
    };

    const GLenum ToGLMagFilterType[] =
    {
        GL_NEAREST, // FT_POINT
        GL_LINEAR   // FT_LINEAR
    };

    const GLenum ToGLWrapType[] =
    {
        GL_REPEAT,          // WT_REPEAT
        GL_CLAMP_TO_BORDER, // WT_CLAMP
        GL_MIRRORED_REPEAT  // WT_MIRROR
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

    Handle CreateBuffer( SizeT size, const void * data, GLenum usage, GLenum target )
    {
        GLuint buffer;
        glGenBuffers( 1, &buffer );
        glBindBuffer( target, buffer );
        glBufferData( target, size, data, usage );
        glBindBuffer( target, 0 );
        return buffer;
    }

    //===================================================================================

    Handle IRenderDevice::CreateVertexBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_ARRAY_BUFFER );
    }

    Handle IRenderDevice::CreateIndexBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_ELEMENT_ARRAY_BUFFER );
    }

    void IRenderDevice::DestroyBuffer( Handle buffer )
    {
        glDeleteBuffers( 1, (GLuint*)&buffer );
    }

    Handle IRenderDevice::CreateVertexArray( const VertexDeclaration& vDecl, Handle vbuffer, Handle ibuffer )
    {
        GLuint varray;
        glGenVertexArrays( 1, &varray );
        glBindVertexArray( varray );

        GLenum error = glGetError();

        glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            const AttribDeclaration& attrib = vDecl.m_attributes[ i ];
            glEnableVertexAttribArray( attrib.m_semantic );
            error = glGetError();
            glVertexAttribPointer( attrib.m_semantic, attrib.m_size, ToGLDataType[ attrib.m_type ], attrib.m_normalized, vDecl.m_size, reinterpret_cast< const void * >( attrib.m_offset ) );
            error = glGetError();
            glDisableVertexAttribArray( attrib.m_semantic );
        }
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        if ( ibuffer )
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibuffer );
            error = glGetError();
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        }

        glBindVertexArray( 0 );
        return varray;
    }

    void IRenderDevice::DestroyVertexArray( Handle varray )
    {
        glDeleteVertexArrays( 1, (GLuint*)&varray );
    }

    U32 IRenderDevice::CreateProgram( const Char * srcBuffers[], SizeT srcSizes[], ShaderType srcTypes[], SizeT count )
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
            if ( logLength > 1024 || logLength < 0 )
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

    void IRenderDevice::DeleteProgram( U32 program )
    {
        glDeleteProgram( program );
    }

    void IRenderDevice::GetProgramBinary( U32 program, void *& binary, SizeT& size )
    {
        GLint binLength;
        glGetProgramiv( program, GL_PROGRAM_BINARY_LENGTH, &binLength );

        size = binLength + sizeof(GLenum);

        binary          = Core::UnknownAllocator::Allocate( size );
        GLenum * fmt    = (GLenum*)binary;
        void * buffer   = fmt + 1;

        glGetProgramBinary( program, binLength, NULL, fmt, buffer );
    }

    U32 IRenderDevice::CreateProgramBinary( const void * binary, SizeT size )
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

    void IRenderDevice::UseProgram( Handle program )
    {
        glUseProgram( (GLuint)program );
    }

    Handle IRenderDevice::CreateTexture( SizeT internalFormat, SizeT externalFormat, SizeT levelCount, Bool compressed, const SizeT * size, const SizeT * width, const SizeT * height, void ** data )
    {
        GLuint texture;
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_2D, texture );

        if ( compressed )
        {
            for ( SizeT i=0; i<levelCount; ++i )
            {
                glCompressedTexImage2D( GL_TEXTURE_2D, i, internalFormat, width[i], height[i], 0, size[i], data[i] );
            }
        }
        else
        {
            glTexStorage2D( GL_TEXTURE_2D, levelCount, internalFormat, width[0], height[0] );
            for ( SizeT i=0; i<levelCount; ++i )
            {
                glTexSubImage2D( GL_TEXTURE_2D, i, 0, 0, width[i], height[i], externalFormat, GL_UNSIGNED_BYTE, data[i] );
            }
        }

        glBindTexture( GL_TEXTURE_2D, 0 );

        return texture;
    }

    void IRenderDevice::DestroyTexture( Handle texture )
    {
        glDeleteTextures( 1, (GLuint*)&texture );
    }

    Handle IRenderDevice::CreateSampler( FilterType min, FilterType mag, MipType mip, WrapType wrap )
    {
        GLuint sampler;
        glGenSamplers( 1, &sampler );
        glSamplerParameteri( sampler    , GL_TEXTURE_MIN_FILTER , ToGLMinFilterType[ mip ][ min ]   );
        glSamplerParameteri( sampler    , GL_TEXTURE_MAG_FILTER , ToGLMagFilterType[ mag ]          );
        glSamplerParameteri( sampler    , GL_TEXTURE_WRAP_S     , ToGLWrapType[ wrap ]              );
        glSamplerParameteri( sampler    , GL_TEXTURE_WRAP_T     , ToGLWrapType[ wrap ]              );

        return sampler;
    }

    void IRenderDevice::DestroySampler( Handle sampler )
    {
        glDeleteSamplers( 1, (GLuint*)&sampler );
    }

    void IRenderDevice::SampleTexture( Handle texture, Handle sampler, SizeT unit )
    {
        glActiveTexture( GL_TEXTURE0 + unit );
        glBindTexture( GL_TEXTURE_2D, texture );
        glBindSampler( unit, sampler );
    }

    void IRenderDevice::BeginGeometry( const VertexDeclaration& vDecl, Handle varray, Handle ibuffer )
    {
        glBindVertexArray( varray );

        if ( ibuffer )
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibuffer );

        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            const AttribDeclaration& attrib = vDecl.m_attributes[ i ];
            glEnableVertexAttribArray( attrib.m_semantic );
        }
    }

    void IRenderDevice::EndGeometry( const VertexDeclaration& vDecl )
    {
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            const AttribDeclaration& attrib = vDecl.m_attributes[ i ];
            glDisableVertexAttribArray( attrib.m_semantic );
        }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        glBindVertexArray( 0 );
    }

    void IRenderDevice::Draw( PrimitiveType primitive, SizeT vertexCount )
    {
        glDrawArrays( ToGLPrimitiveType[ primitive ], 0, vertexCount );
    }

    void IRenderDevice::DrawIndexed( PrimitiveType primitive, SizeT indexCount, DataType indexType )
    {
        glDrawElements( ToGLPrimitiveType[ primitive ], indexCount, ToGLDataType[ indexType ], (GLvoid*)0 );
    }

    void IRenderDevice::ClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        glEnable( GL_CULL_FACE );
        glEnable( GL_DEPTH_TEST );

        glClearColor( r, g, b, a );
        glClearDepth( 1.0 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    void IRenderDevice::SetSRGBWrite( Bool enable )
    {
        if ( enable )
            glEnable( GL_FRAMEBUFFER_SRGB );
        else
            glDisable( GL_FRAMEBUFFER_SRGB );
    }
}

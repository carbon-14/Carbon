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

    VertexArray * IRenderDevice::CreateVertexArray(
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

    void IRenderDevice::DestroyVertexArray( VertexArray * v )
    {
        CARBON_ASSERT( v );

        glDeleteVertexArrays( 1, &v->m_va );
        glDeleteBuffers( 1, &v->m_vb );
        glDeleteBuffers( 1, &v->m_ib );

        Core::UnknownAllocator::Deallocate( v );
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
        GLuint t = texture;
        glDeleteTextures( 1, &t );
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
        GLuint s = sampler;
        glDeleteSamplers( 1, &s );
    }

    void IRenderDevice::SampleTexture( Handle texture, Handle sampler, SizeT unit )
    {
        glActiveTexture( GL_TEXTURE0 + unit );
        glBindTexture( GL_TEXTURE_2D, texture );
        glBindSampler( unit, sampler );
    }

    void IRenderDevice::Draw( PrimitiveType primitive, VertexArray * va )
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

    void IRenderDevice::SetViewport( U32 x, U32 y, U32 w, U32 h )
    {
        glViewport( x, y, w, h );
    }

    void IRenderDevice::ClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        glClearColor( r, g, b, a );
        glClear( GL_COLOR_BUFFER_BIT );
    }

    void IRenderDevice::SetSRGBWrite( Bool enable )
    {
        if ( enable )
            glEnable( GL_FRAMEBUFFER_SRGB );
        else
            glDisable( GL_FRAMEBUFFER_SRGB );
    }
}

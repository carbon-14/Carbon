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

    const GLenum ToGLBufferAccess[] =
    {
        GL_READ_ONLY,       // BA_READ_ONLY
        GL_WRITE_ONLY,      // BA_WRITE_ONLY
        GL_READ_WRITE       // BU_READ_WRITE
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

    const GLenum ToGLClearMask[] =
    {
        0,
        GL_COLOR_BUFFER_BIT,                                                // CM_COLOR
        GL_DEPTH_BUFFER_BIT,                                                // CM_DEPTH
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_STENCIL_BUFFER_BIT,                                              // CM_STENCIL
        GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
    };

    const GLenum ToGLFunction[] =
    {
        GL_NEVER,       // F_NEVER
        GL_LESS,        // F_LESS
        GL_LEQUAL,      // F_LEQUAL
        GL_GREATER,     // F_GREATER
        GL_GEQUAL,      // F_GEQUAL
        GL_EQUAL,       // F_EQUAL
        GL_NOTEQUAL,    // F_NOTEQUAL
        GL_ALWAYS       // F_ALWAYS
    };

    const GLenum ToGLOperation[] =
    {
        GL_KEEP,        // O_KEEP
        GL_ZERO,        // O_ZERO
        GL_REPLACE,     // O_REPLACE
        GL_INCR,        // O_INCR
        GL_INCR_WRAP,   // O_INCR_WRAP
        GL_DECR,        // O_DECR
        GL_DECR_WRAP,   // O_DECR_WRAP
        GL_INVERT       // O_INVERT
    };

    const GLenum ToGLCullFace[] =
    {
        GL_FRONT,       // CF_FRONT
        GL_BACK         // CF_BACK
    };

    const GLenum ToGLBlendFunction[] =
    {
        GL_ZERO,                        // BF_ZERO
        GL_ONE,                         // BF_ONE,
        GL_SRC_COLOR,                   // BF_SRC_COLOR
        GL_ONE_MINUS_SRC_COLOR,         // BF_ONE_MINUS_SRC_COLOR
        GL_DST_COLOR,                   // BF_DST_COLOR
        GL_ONE_MINUS_DST_COLOR,         // BF_ONE_MINUS_DST_COLOR
        GL_SRC_ALPHA,                   // BF_SRC_ALPHA
        GL_ONE_MINUS_SRC_ALPHA,         // BF_ONE_MINUS_SRC_ALPHA
        GL_DST_ALPHA,                   // BF_DST_ALPHA
        GL_ONE_MINUS_DST_ALPHA,         // BF_ONE_MINUS_DST_ALPHA
        GL_CONSTANT_COLOR,              // BF_CONSTANT_COLOR
        GL_ONE_MINUS_CONSTANT_COLOR,    // BF_ONE_MINUS_CONSTANT_COLOR
        GL_CONSTANT_ALPHA,              // BF_CONSTANT_ALPHA
        GL_ONE_MINUS_CONSTANT_ALPHA,    // BF_ONE_MINUS_CONSTANT_ALPHA
        GL_SRC_ALPHA_SATURATE,          // BF_SRC_ALPHA_SATURATE
        GL_SRC1_COLOR,                  // BF_SRC1_COLOR
        GL_ONE_MINUS_SRC1_COLOR,        // BF_ONE_MINUS_SRC1_COLOR
        GL_SRC1_ALPHA,                  // BF_SR1_ALPHA
        GL_ONE_MINUS_SRC1_ALPHA         // BF_ONE_MINUS_SRC1_ALPHA
    };

    const GLenum ToGLBlendMode[] =
    {
        GL_FUNC_ADD,                // BM_ADD
        GL_FUNC_SUBTRACT,           // BM_SUBSTRACT
        GL_FUNC_REVERSE_SUBTRACT,   // BM_REVERSE_SUBSTRACT
        GL_MIN,                     // BM_MIN
        GL_MAX                      // BM_MAX
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

    Handle IRenderDevice::CreateUniformBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_UNIFORM_BUFFER );
    }

    void IRenderDevice::DestroyBuffer( Handle buffer )
    {
        glDeleteBuffers( 1, (GLuint*)&buffer );
    }

    void * IRenderDevice::MapUniformBuffer( Handle buffer, BufferAccess access )
    {
        glBindBuffer( GL_UNIFORM_BUFFER, buffer );
        return glMapBuffer( GL_UNIFORM_BUFFER, ToGLBufferAccess[ access ] );
    }

    void IRenderDevice::UnmapUniformBuffer( )
    {
        GLboolean test = glUnmapBuffer( GL_UNIFORM_BUFFER );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }

    void IRenderDevice::BindUniformBuffer( Handle ubuffer, SizeT location )
    {
        glBindBufferBase( GL_UNIFORM_BUFFER, location, ubuffer );
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

    void IRenderDevice::BindTexture( Handle texture, SizeT unit )
    {
        glActiveTexture( GL_TEXTURE0 + unit );
        glBindTexture( GL_TEXTURE_2D, texture );
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

    void IRenderDevice::BindSampler( Handle sampler, SizeT unit )
    {
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

    void IRenderDevice::SetClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        glClearColor( r, g, b, a );
    }

    void IRenderDevice::SetClearDepth( F32 depth )
    {
        glClearDepth( depth );
    }

    void IRenderDevice::SetClearStencil( U8 s )
    {
        glClearStencil( s );
    }

    void IRenderDevice::Clear( U32 mask )
    {
        glClear( ToGLClearMask[mask] );
    }

    void IRenderDevice::SetColorWrite( U32 mask )
    {
        glColorMask( mask & CM_RED, mask & CM_GREEN, mask & CM_BLUE, mask & CM_ALPHA );
    }

    void IRenderDevice::SetDepthWrite( Bool enable )
    {
        glDepthMask( enable );
    }

    void IRenderDevice::SetStencilWrite( U8 mask )
    {
        glStencilMask( mask );
    }

    void IRenderDevice::EnableDepthTest( Bool enable )
    {
        enable ? glEnable( GL_DEPTH_TEST ) : glDisable( GL_DEPTH_TEST );
    }

    void IRenderDevice::SetDepthFunc( Function f )
    {
        glDepthFunc( ToGLFunction[f] );
    }

    void IRenderDevice::SetStencilOp( Operation stencilFail, Operation depthFail, Operation depthPass )
    {
        glStencilOp( ToGLOperation[ stencilFail ], ToGLOperation[ depthFail ], ToGLOperation[ depthPass ] );
    }

    void IRenderDevice::SetStencilFunc( Function f, U8 ref, U8 mask )
    {
        glStencilFunc( ToGLFunction[f], ref, mask );
    }

    void IRenderDevice::EnableCullFace( Bool enable )
    {
        enable ? glEnable( GL_CULL_FACE ) : glDisable( GL_CULL_FACE );
    }

    void IRenderDevice::SetCullFace( CullFace face )
    {
        glCullFace( ToGLCullFace[ face ] );
    }

    void IRenderDevice::SetBlendColor( F32 r, F32 g, F32 b, F32 a )
    {
        glBlendColor( r, g, b, a );
    }

    void IRenderDevice::SetBlendFunc( BlendFunction src, BlendFunction dst )
    {
        glBlendFunc( ToGLBlendFunction[ src ], ToGLBlendFunction[ dst ] );
    }

    void IRenderDevice::SetBlendFuncSeparate( BlendFunction srcRGB, BlendFunction dstRGB, BlendFunction srcAlpha, BlendFunction dstAlpha )
    {
        glBlendFuncSeparate( ToGLBlendFunction[ srcRGB ], ToGLBlendFunction[ dstRGB ], ToGLBlendFunction[ srcAlpha ], ToGLBlendFunction[ dstAlpha ] );
    }

    void IRenderDevice::SetBlendMode( BlendMode mode )
    {
        glBlendEquation( ToGLBlendMode[ mode ] );
    }

    void IRenderDevice::SetSRGBWrite( Bool enable )
    {
        enable ? glEnable( GL_FRAMEBUFFER_SRGB ) : glDisable( GL_FRAMEBUFFER_SRGB );
    }
}

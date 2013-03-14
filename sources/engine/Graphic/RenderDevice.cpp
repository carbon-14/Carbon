#include "Graphic/RenderDevice.h"

#include "Core/MemoryManager.h"

#include "Core/Assert.h"
#include "Core/Trace.h"

namespace Graphic
{
    const GLenum ToGLShaderType[] =
    {
        GL_VERTEX_SHADER,   // ST_VERTEX_SHADER
        GL_FRAGMENT_SHADER, // ST_FRAGMENT_SHADER
        GL_GEOMETRY_SHADER  // ST_GEOMETRY_SHADER
    };

    const GLenum ToGLBufferUsage[] =
    {
        GL_STREAM_DRAW,     // BU_STREAM
        GL_STATIC_DRAW,     // BU_STATIC
        GL_DYNAMIC_DRAW     // BU_DYNAMIC

    };

    const GLenum ToGLBufferAccess[] =
    {
        GL_READ_ONLY,       // BA_READ_ONLY
        GL_WRITE_ONLY,      // BA_WRITE_ONLY
        GL_READ_WRITE       // BU_READ_WRITE
    };

    const GLenum ToGLTextureFormat[] =
    {
        GL_R8,                  // TF_R8
        GL_R16,                 // TF_R16
        GL_R16F,                // TF_R16F
        GL_R32F,                // TF_R32F
        GL_RG8,                 // TF_RG8
        GL_RG16,                // TF_RG16
        GL_RG16F,               // TF_RG16F
        GL_RGBA8,               // TF_RGBA8
        GL_SRGB8_ALPHA8,        // TF_SRGBA8
        GL_RGBA16,              // TF_RGBA16
        GL_RGBA16F,             // TF_RGBA16F
        GL_DEPTH24_STENCIL8     // TF_D24S8
    };

    const GLenum ToGLTextureTargetCube[] =
    {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    const GLenum ToGLFramebufferTarget[] =
    {
        GL_READ_FRAMEBUFFER,    // FT_READ
        GL_DRAW_FRAMEBUFFER,    // FT_DRAW
        GL_FRAMEBUFFER          // FT_BOTH
    };

    const GLenum ToGLFramebufferAttachment[] =
    {
        GL_COLOR_ATTACHMENT0,           // FA_COLOR0
        GL_COLOR_ATTACHMENT1,           // FA_COLOR1
        GL_COLOR_ATTACHMENT2,           // FA_COLOR2
        GL_COLOR_ATTACHMENT3,           // FA_COLOR3
        GL_DEPTH_ATTACHMENT,            // FA_DEPTH
        GL_STENCIL_ATTACHMENT,          // FA_STENCIL
        GL_DEPTH_STENCIL_ATTACHMENT,    // FA_DEPTH_STENCIL
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

    const GLenum ToGLFilterType[] =
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

    const GLenum ToGLRenderMask[] =
    {
        0,
        GL_COLOR_BUFFER_BIT,                                                // RM_COLOR
        GL_DEPTH_BUFFER_BIT,                                                // RM_DEPTH
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_STENCIL_BUFFER_BIT,                                              // RM_STENCIL
        GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
    };

    const GLenum ToGLColorBuffer[] =
    {
        GL_COLOR_ATTACHMENT0,   // CB_COLOR0
        GL_COLOR_ATTACHMENT1,   // CB_COLOR1
        GL_COLOR_ATTACHMENT2,   // CB_COLOR2
        GL_COLOR_ATTACHMENT3,   // CB_COLOR3
        GL_BACK_LEFT            // CB_BACK
    };

    const GLenum ToGLColorBuffers[][4] =
    {
        { GL_NONE               , GL_NONE               , GL_NONE               , GL_NONE               },  // unused
        { GL_COLOR_ATTACHMENT0  , GL_NONE               , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT1  , GL_NONE               , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT1  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT2  , GL_NONE               , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT2  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT2  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT2  , GL_NONE               },
        { GL_COLOR_ATTACHMENT3  , GL_NONE               , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT3  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT3  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT3  , GL_NONE               },
        { GL_COLOR_ATTACHMENT2  , GL_COLOR_ATTACHMENT3  , GL_NONE               , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT2  , GL_COLOR_ATTACHMENT3  , GL_NONE               },
        { GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT2  , GL_COLOR_ATTACHMENT3  , GL_NONE               },
        { GL_COLOR_ATTACHMENT0  , GL_COLOR_ATTACHMENT1  , GL_COLOR_ATTACHMENT2  , GL_COLOR_ATTACHMENT3  },
        { GL_BACK_LEFT          , GL_NONE               , GL_NONE               , GL_NONE               }
    };

    const GLenum ToGLColorBufferCount[] =   { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1 };

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

    static GLuint   s_samplerCache[ s_maxTextureUnitCount ];
    static GLuint   s_activeTexture;
    static GLuint   s_textureCache[ s_maxTextureUnitCount ];
    static GLuint   s_uniformCache[ s_maxUniformBufferCount ];
    static GLuint   s_framebufferCache;
    static GLuint   s_programCache;

    //===================================================================================

    void TraceProgramInfoLog( GLuint program )
    {
        GLint logLength;
        GLchar buffer[ 1024 ];

        glGetShaderiv( program, GL_INFO_LOG_LENGTH, &logLength );
        if ( logLength > 1024 || logLength < 0 )
        {
            logLength = 1024;
        }

        glGetProgramInfoLog( program, logLength, 0, buffer );
        buffer[ logLength ] = '\n';
        CARBON_TRACE( buffer );
    }

    void TraceShaderInfoLog( GLuint shader )
    {
        GLint logLength;
        GLchar buffer[ 1024 ];

        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
        if ( logLength > 1024 )
        {
            logLength = 1024;
        }

        glGetShaderInfoLog( shader, logLength, 0, buffer );
        buffer[ logLength ] = '\n';
		CARBON_TRACE( buffer );
    }

    Handle CreateBuffer( SizeT size, const void * data, GLenum usage, GLenum target )
    {
        GLuint buffer;

        glGenBuffers( 1, &buffer );
        glBindBuffer( target, buffer );
        glBufferData( target, size, data, usage );

        return buffer;
    }

    void * MapBuffer( GLuint buffer, GLenum access, GLenum target )
    {
        glBindBuffer( target, buffer );

        return glMapBuffer( target, access );
    }

    //===================================================================================

    Handle IRenderDevice::CreateVertexBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_ARRAY_BUFFER );
    }

    void * IRenderDevice::MapVertexBuffer( Handle buffer, BufferAccess access )
    {
        return MapBuffer( buffer, ToGLBufferAccess[ access ], GL_ARRAY_BUFFER ); 
    }

    void IRenderDevice::UnmapVertexBuffer( )
    {
        glUnmapBuffer( GL_ARRAY_BUFFER );
    }

    Handle IRenderDevice::CreateIndexBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_ELEMENT_ARRAY_BUFFER );
    }

    void * IRenderDevice::MapIndexBuffer( Handle buffer, BufferAccess access )
    {
        return MapBuffer( buffer, ToGLBufferAccess[ access ], GL_ELEMENT_ARRAY_BUFFER ); 
    }

    void IRenderDevice::UnmapIndexBuffer( )
    {
        glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
    }

    Handle IRenderDevice::CreateUniformBuffer( SizeT size, const void * data, BufferUsage usage )
    {
        return CreateBuffer( size, data, ToGLBufferUsage[ usage ], GL_UNIFORM_BUFFER );
    }

    void * IRenderDevice::MapUniformBuffer( Handle buffer, BufferAccess access )
    {
        return MapBuffer( buffer, ToGLBufferAccess[ access ], GL_UNIFORM_BUFFER ); 
    }

    void IRenderDevice::UnmapUniformBuffer( )
    {
        glUnmapBuffer( GL_UNIFORM_BUFFER );
    }

    void IRenderDevice::BindUniformBuffer( Handle ubuffer, SizeT location )
    {
        if ( ubuffer != s_uniformCache[location] )
        {
            glBindBufferBase( GL_UNIFORM_BUFFER, location, ubuffer );
            s_uniformCache[location] = ubuffer;
        }
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
        glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibuffer );

        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            const AttribDeclaration& attrib = vDecl.m_attributes[ i ];
            glEnableVertexAttribArray( attrib.m_semantic );
            glVertexAttribPointer( attrib.m_semantic, attrib.m_size, ToGLDataType[ attrib.m_type ], attrib.m_normalized, vDecl.m_size, reinterpret_cast< const void * >( attrib.m_offset ) );
        }

        glBindVertexArray( 0 );

        return varray;
    }

    void IRenderDevice::DestroyVertexArray( Handle varray )
    {
        glDeleteVertexArrays( 1, (GLuint*)&varray );
    }

    U32 IRenderDevice::CreateProgram( const Char * srcBuffers[], ShaderType srcTypes[], SizeT count, const Char * headers )
    {
        CARBON_ASSERT( count > 0 );

        GLuint program = glCreateProgram();

        const Char * sources[] = { headers, 0 };

        for ( SizeT i=0; i<count; ++i )
        {
            sources[1] = srcBuffers[ i ];

            GLuint shader = glCreateShader( ToGLShaderType[ srcTypes[ i ] ] );
            glShaderSource( shader, 2, sources, 0 );
            glCompileShader( shader );

            GLint compileStatus = GL_FALSE;
		    glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

            if ( compileStatus == GL_FALSE )
            {
                TraceShaderInfoLog( shader );
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
            TraceProgramInfoLog( program );
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

        binary          = UnknownAllocator::Allocate( size );
        GLenum * fmt    = (GLenum*)binary;
        void * buffer   = fmt + 1;

        glGetProgramBinary( program, binLength, 0, fmt, buffer );
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
            TraceProgramInfoLog( program );
            return 0;
		}

        return program;
    }

    void IRenderDevice::UseProgram( Handle program )
    {
        if ( program != s_programCache )
        {
            glUseProgram( (GLuint)program );
            s_programCache = program;
        }
    }

    Handle IRenderDevice::CreateTexture( SizeT internalFormat, SizeT externalFormat, SizeT levelCount, Bool compressed, const SizeT * size, const SizeT * width, const SizeT * height, void ** data )
    {
        GLuint texture;
        glGenTextures( 1, &texture );

        glBindTexture( GL_TEXTURE_2D, texture );
        s_textureCache[s_activeTexture] = texture;

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

        return texture;
    }

    Handle IRenderDevice::CreateRenderTexture( TextureFormat format, SizeT width, SizeT height )
    {
        GLuint texture;
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_2D, texture );
        s_textureCache[s_activeTexture] = texture;
        glTexStorage2D( GL_TEXTURE_2D, 1, ToGLTextureFormat[ format ], width, height );
        return texture;
    }

    Handle IRenderDevice::CreateRenderTextureCube( TextureFormat format, SizeT size )
    {
        GLuint texture;
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_CUBE_MAP, texture );
        s_textureCache[s_activeTexture] = texture;
        glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, ToGLTextureFormat[ format ], size, size );
        return texture;
    }

    void IRenderDevice::DestroyTexture( Handle texture )
    {
        glDeleteTextures( 1, (GLuint*)&texture );
    }

    void IRenderDevice::BindTexture( Handle texture, SizeT unit )
    {
        if ( unit != s_activeTexture )
        {
            glActiveTexture( GL_TEXTURE0 + unit );
            s_activeTexture = unit;
        }
        if ( texture != s_textureCache[unit] )
        {
            glBindTexture( GL_TEXTURE_2D, texture );
            s_textureCache[unit] = texture;
        }
    }

    void IRenderDevice::BindTextureCube( Handle texture, SizeT unit )
    {
        if ( unit != s_activeTexture )
        {
            glActiveTexture( GL_TEXTURE0 + unit );
            s_activeTexture = unit;
        }
        if ( texture != s_textureCache[unit] )
        {
            glBindTexture( GL_TEXTURE_CUBE_MAP, texture );
            s_textureCache[unit] = texture;
        }
    }

    Handle IRenderDevice::CreateSampler( FilterType min, FilterType mag, MipType mip, WrapType wrap )
    {
        GLuint sampler;
        glGenSamplers( 1, &sampler );
        glSamplerParameteri( sampler    , GL_TEXTURE_MIN_FILTER , ToGLMinFilterType[ mip ][ min ]   );
        glSamplerParameteri( sampler    , GL_TEXTURE_MAG_FILTER , ToGLFilterType[ mag ]             );
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
        if ( sampler != s_samplerCache[unit] )
        {
            glBindSampler( unit, sampler );
            s_samplerCache[unit] = sampler;
        }
    }

    Handle IRenderDevice::CreateRenderbuffer( TextureFormat format, SizeT width, SizeT height )
    {
        GLuint renderbuffer;
        glGenRenderbuffers( 1, &renderbuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer );
        glRenderbufferStorage( GL_RENDERBUFFER, ToGLTextureFormat[ format ], width, height );
        return renderbuffer;
    }

    Handle IRenderDevice::CreateRenderbufferMultisample( SizeT samples, TextureFormat format, SizeT width, SizeT height )
    {
        GLuint renderbuffer;
        glGenRenderbuffers( 1, &renderbuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, renderbuffer );
        glRenderbufferStorageMultisample( GL_RENDERBUFFER, samples, ToGLTextureFormat[ format ], width, height );
        return renderbuffer;
    }

    void IRenderDevice::DestroyRenderbuffer( Handle renderbuffer )
    {
        glDeleteRenderbuffers( 1, (GLuint*)&renderbuffer );
    }

    Handle IRenderDevice::CreateFramebuffer()
    {
        GLuint frameBuffer;
        glGenFramebuffers( 1, &frameBuffer );
        return frameBuffer;
    }

    void IRenderDevice::DestroyFramebuffer( Handle framebuffer )
    {
        glDeleteFramebuffers( 1, (GLuint*)&framebuffer );
    }

    void IRenderDevice::BindFramebuffer( Handle framebuffer, FramebufferTarget target )
    {
        if ( framebuffer != s_framebufferCache )
        {
            glBindFramebuffer( ToGLFramebufferTarget[ target ], framebuffer );
            s_framebufferCache = framebuffer;
        }
    }

    void IRenderDevice::AttachTexture( FramebufferTarget target, FramebufferAttachment attachment, Handle texture, SizeT level )
    {
        glFramebufferTexture2D( ToGLFramebufferTarget[ target ], ToGLFramebufferAttachment[ attachment ], GL_TEXTURE_2D, texture, level );
    }

    void IRenderDevice::AttachTextureCube( FramebufferTarget target, FramebufferAttachment attachment, CubeFace face, Handle texture, SizeT level )
    {
        glFramebufferTexture2D( ToGLFramebufferTarget[ target ], ToGLFramebufferAttachment[ attachment ], ToGLTextureTargetCube[ face ], texture, level );
    }

    void IRenderDevice::AttachRenderbuffer( FramebufferTarget target, FramebufferAttachment attachment, Handle renderbuffer )
    {
        glFramebufferRenderbuffer( ToGLFramebufferTarget[ target ], ToGLFramebufferAttachment[ attachment ], GL_RENDERBUFFER, renderbuffer );
    }

    void IRenderDevice::BlitFramebuffer( SizeT srcX, SizeT srcY, SizeT srcWidth, SizeT srcHeight, SizeT dstX, SizeT dstY, SizeT dstWidth, SizeT dstHeight, U32 mask, FilterType filter )
    {
        glBlitFramebuffer( srcX, srcY, srcWidth, srcHeight, dstX, dstY, dstWidth, dstHeight, ToGLRenderMask[ mask ], ToGLFilterType[ filter ] );
    }

    void IRenderDevice::DrawBuffer( U32 mask )
    {
        glDrawBuffers( ToGLColorBufferCount[ mask ], ToGLColorBuffers[ mask ] );
    }

    void IRenderDevice::ReadBuffer( ColorBuffer colorBuffer )
    {
        glReadBuffer( ToGLColorBuffer[ colorBuffer ] );
    }

    void IRenderDevice::BeginGeometry( Handle varray )
    {
        glBindVertexArray( varray );
    }

    void IRenderDevice::EndGeometry()
    {
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
        glClear( ToGLRenderMask[mask] );
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

    void IRenderDevice::EnableStencilTest( Bool enable )
    {
        enable ? glEnable( GL_STENCIL_TEST ) : glDisable( GL_STENCIL_TEST );
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

    void IRenderDevice::EnableBlend( Bool enable )
    {
        glEnable( GL_BLEND );
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

    void IRenderDevice::SetViewport( SizeT x, SizeT y, SizeT width, SizeT height )
    {
        glViewport( x, y, width, height );
    }

    void IRenderDevice::CatchError()
    {
        switch( glGetError() )
        {
        case GL_INVALID_ENUM :
            CARBON_TRACE( "GL ERROR : Invalid enum\n" );
            break;
        case GL_INVALID_VALUE :
            CARBON_TRACE( "GL ERROR : Invalid value\n" );
            break;
        case GL_INVALID_OPERATION :
            CARBON_TRACE( "GL ERROR : Invalid operation\n" );
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION :
            CARBON_TRACE( "GL ERROR : Invalid framebuffer operation\n" );
            break;
        case GL_OUT_OF_MEMORY :
            CARBON_TRACE( "GL ERROR : Out of memory\n" );
            break;
        case GL_STACK_UNDERFLOW :
            CARBON_TRACE( "GL ERROR : Stack underflow\n" );
            break;
        case GL_STACK_OVERFLOW :
            CARBON_TRACE( "GL ERROR : Stack overflow\n" );
            break;
        }
    }

    void IRenderDevice::ClearCache()
    {
        for ( SizeT i=0; i<s_maxTextureUnitCount; ++i )
        {
            glActiveTexture( GL_TEXTURE0 + i );
            glBindTexture( GL_TEXTURE_2D, 0 );
            glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
            s_samplerCache[i] = 0;
            s_textureCache[i] = 0;
        }

        glActiveTexture( GL_TEXTURE0 );
        s_activeTexture = 0;

        for ( SizeT i=0; i<s_maxUniformBufferCount; ++i )
        {
            glBindBufferBase( GL_UNIFORM_BUFFER, i, 0 );
            s_uniformCache[i] = 0;
        }

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        s_framebufferCache = 0;

        glUseProgram( 0 );
        s_programCache = 0;
    }
}

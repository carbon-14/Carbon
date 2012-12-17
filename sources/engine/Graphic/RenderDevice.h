#pragma once
#ifndef _GRAPHIC_RENDERDEVICE_H
#define _GRAPHIC_RENDERDEVICE_H

#include "Graphic/DLL.h"

#include "Core/Types.h"

namespace Graphic
{
    enum ShaderType
    {
        ST_VERTEX_SHADER = 0,
        ST_FRAGMENT_SHADER,
        ST_GEOMETRY_SHADER,
        ST_COUNT
    };

    enum DataType
    {
        DT_S8 = 0,
        DT_U8,
        DT_S16,
        DT_U16,
        DT_F16,
        DT_S32,
        DT_U32,
        DT_F32,
        DT_F64,
        DT_S2_10_10_10,
        DT_U2_10_10_10
    };

    const SizeT DataTypeSize[] = { 1, 1, 2, 2, 2, 4, 4, 4, 8, 4, 4 };

    enum VertexSemantic
    {
        VS_POSITION = 0,
        VS_NORMAL,
        VS_TANGENT,
        VS_BINORMAL,
        VS_COLOR,
        VS_TEXCOORD0,
        VS_TEXCOORD1,
        VS_COUNT
    };

    enum BufferUsage
    {
        BU_STATIC = 0,
        BU_DYNAMIC,
        BU_STREAM
    };

    enum BufferAccess
    {
        BA_READ_ONLY,
        BA_WRITE_ONLY,
        BA_READ_WRITE
    };

    struct AttribDeclaration
    {
        VertexSemantic  m_semantic;
        DataType        m_type;
        SizeT           m_size;
        Bool            m_normalized;
        SizeT           m_offset;
    };

    static const SizeT s_MaxVertexAttribCount = 16;
    struct VertexDeclaration
    {
        AttribDeclaration   m_attributes[ s_MaxVertexAttribCount ];
        SizeT               m_size;
        SizeT               m_count;
    };

    enum PrimitiveType
    {
        PT_POINTS,
        PT_LINES,
        PT_TRIANGLES
    };

    enum FilterType
    {
        FT_POINT,
        FT_LINEAR
    };

    enum MipType
    {
        MT_NONE,
        MT_POINT,
        MT_LINEAR
    };

    enum WrapType
    {
        WT_REPEAT,
        WT_CLAMP,
        WT_MIRROR
    };

    enum ClearMode
    {
        CM_COLOR            = 1 << 0,
        CM_DEPTH            = 1 << 1,
        CM_STENCIL          = 1 << 2
    };

    enum ColorMode
    {
        CM_RED      = 1 << 0,
        CM_GREEN    = 1 << 1,
        CM_BLUE     = 1 << 2,
        CM_ALPHA    = 1 << 3,
        CM_RGB      = CM_RED | CM_GREEN | CM_BLUE,
        CM_RGBA     = CM_RGB | CM_ALPHA
    };

    enum Func
    {
        F_NEVER,
        F_LESS,
        F_EQUAL,
        F_LEQUAL,
        F_GREATER,
        F_NOTEQUAL,
        F_GEQUAL,
        F_ALWAYS
    };

    enum Operator
    {
        O_KEEP,
        O_ZERO,
        O_REPLACE,
        O_INCR,
        O_INCR_WRAP,
        O_DECR,
        O_DECR_WRAP,
        O_INVERT
    };

    enum CullFace
    {
        CF_FRONT,
        CF_BACK
    };

    struct VertexArray;

    typedef U32 Handle;

    class _GraphicExport IRenderDevice
    {
    public:
        static Handle           CreateVertexBuffer( SizeT size, const void * data, BufferUsage usage );
        static Handle           CreateIndexBuffer( SizeT size, const void * data, BufferUsage usage );
        static Handle           CreateUniformBuffer( SizeT size, const void * data, BufferUsage usage );
        static void             DestroyBuffer( Handle buffer );

        static void *           MapUniformBuffer( Handle buffer, BufferAccess access );
        static void             UnmapUniformBuffer( );

        static Handle           CreateVertexArray( const VertexDeclaration& vDecl, Handle vbuffer, Handle ibuffer );
        static void             DestroyVertexArray( Handle varray );

        static Handle           CreateProgram( const Char * srcBuffers[], SizeT srcSizes[], ShaderType srcTypes[], SizeT count );
        static void             DeleteProgram( Handle program );
        static void             GetProgramBinary( Handle program, void *& binary, SizeT& size );
        static Handle           CreateProgramBinary( const void * binary, SizeT size );
        static void             UseProgram( Handle program );

        static Handle           CreateTexture( SizeT internalFormat, SizeT externalFormat, SizeT levelCount, Bool compressed, const SizeT * size, const SizeT * width, const SizeT * height, void ** data );
        static void             DestroyTexture( Handle texture );
        static Handle           CreateSampler( FilterType min, FilterType mag, MipType mip, WrapType wrap );
        static void             DestroySampler( Handle sampler );
        static void             SampleTexture( Handle texture, Handle sampler, SizeT unit );

        static void             BindUniformBuffer( Handle ubuffer, SizeT location );

        static void             BeginGeometry( const VertexDeclaration& vDecl, Handle varray, Handle ibuffer );
        static void             EndGeometry( const VertexDeclaration& vDecl );

        static void             Draw( PrimitiveType primitive, SizeT vertexCount );
        static void             DrawIndexed( PrimitiveType primitive, SizeT indexCount, DataType indexType );

        // Render State

        static void             SetClearColor( F32 r, F32 g, F32 b, F32 a );
        static void             SetClearDepth( F32 d );
        static void             SetClearStencil( U8 s );
        static void             Clear( U32 mask );

        static void             SetColorWrite( U32 mask );
        static void             SetDepthWrite( Bool enable );
        static void             SetStencilWrite( U8 mask );

        static void             EnableDepthTest( Bool enable );
        static void             SetDepthFunc( Func f );
        static void             SetStencilOp( Operator stencilFail, Operator depthFail, Operator depthPass );
        static void             SetStencilFunc( Func f, U8 ref, U8 mask );

        static void             EnableCullFace( Bool enable );
        static void             SetCullFace( CullFace face );

        //static void             SetBlendFunc( BlendFunc src, BlendFunc dst );
        //static void             SetBlendFuncSeparate( BlendFunc srcRGB, BlendFunc dstRGB, BlendFunc srcAlpha, BlendFunc dstAlpha );
        //static void             SetBlendMode( BlendMode mode );

        static void             SetSRGBWrite( Bool enable );
    };
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Graphic/ps/win32/RenderDevice.inl"
#else
    #error RenderDevice not implemented !
#endif

#endif // _GRAPHIC_RENDERDEVICE_H

#pragma once
#ifndef _GRAPHIC_RENDERDEVICE_H
#define _GRAPHIC_RENDERDEVICE_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

namespace Graphic
{
    class _GraphicExport IRenderDevice
    {
    public:
        static Handle           CreateVertexBuffer( SizeT size, const void * data, BufferUsage usage );
        static void *           MapVertexBuffer( Handle buffer, BufferAccess access );
        static void             UnmapVertexBuffer( );
        static Handle           CreateIndexBuffer( SizeT size, const void * data, BufferUsage usage );
        static void *           MapIndexBuffer( Handle buffer, BufferAccess access );
        static void             UnmapIndexBuffer( );
        static Handle           CreateUniformBuffer( SizeT size, const void * data, BufferUsage usage );
        static void *           MapUniformBuffer( Handle buffer, BufferAccess access );
        static void             UnmapUniformBuffer( );
        static void             BindUniformBuffer( Handle ubuffer, SizeT location );
        static void             DestroyBuffer( Handle buffer );

        static Handle           CreateVertexArray( const VertexDeclaration& vDecl, Handle vbuffer, Handle ibuffer = 0 );
        static void             DestroyVertexArray( Handle varray );

        static Handle           CreateProgram( const Char * srcBuffers[], ShaderType srcTypes[], SizeT count, const Char * headers );
        static void             DeleteProgram( Handle program );
        static void             GetProgramBinary( Handle program, void *& binary, SizeT& size );
        static Handle           CreateProgramBinary( const void * binary, SizeT size );
        static void             UseProgram( Handle program );

        static Handle           CreateTexture( SizeT internalFormat, SizeT externalFormat, SizeT levelCount, Bool compressed, const SizeT * size, const SizeT * width, const SizeT * height, void ** data );
        static Handle           CreateRenderTexture( TextureFormat format, SizeT width, SizeT height );
        static Handle           CreateRenderTextureCube( TextureFormat format, SizeT size );
        static void             DestroyTexture( Handle texture );
        static void             BindTexture( Handle texture, SizeT unit );
        static void             BindTextureCube( Handle texture, SizeT unit );

        static Handle           CreateSampler( FilterType min, FilterType mag, MipType mip, WrapType wrap );
        static void             DestroySampler( Handle sampler );
        static void             BindSampler( Handle sampler, SizeT unit );

        static Handle           CreateRenderbuffer( TextureFormat format, SizeT width, SizeT height );
        static Handle           CreateRenderbufferMultisample( SizeT samples, TextureFormat format, SizeT width, SizeT height );
        static void             DestroyRenderbuffer( Handle renderbufer );

        static Handle           CreateFramebuffer();
        static void             DestroyFramebuffer( Handle framebuffer );
        static void             BindFramebuffer( Handle framebuffer, FramebufferTarget target );
        static void             AttachTexture( FramebufferTarget target, FramebufferAttachment attachment, Handle texture, SizeT level );
        static void             AttachTextureCube( FramebufferTarget target, FramebufferAttachment attachment, CubeFace face, Handle texture, SizeT level );
        static void             AttachRenderbuffer( FramebufferTarget target, FramebufferAttachment attachment, Handle renderbuffer );
        static void             BlitFramebuffer( SizeT srcX, SizeT srcY, SizeT srcWidth, SizeT srcHeight, SizeT dstX, SizeT dstY, SizeT dstWidth, SizeT dstHeight, U32 mask, FilterType filter );

        static void             DrawBuffer( U32 mask );
        static void             ReadBuffer( ColorBuffer colorBuffer );

        static void             BeginGeometry( Handle varray );
        static void             EndGeometry();

        static void             Draw( PrimitiveType primitive, SizeT vertexCount );
        static void             DrawIndexed( PrimitiveType primitive, SizeT indexCount, DataType indexType );

        static void             SetClearColor( F32 r, F32 g, F32 b, F32 a );
        static void             SetClearDepth( F32 d );
        static void             SetClearStencil( U8 s );
        static void             Clear( U32 mask );

        // Render State

        static void             SetColorWrite( U32 mask );
        static void             SetDepthWrite( Bool enable );
        static void             SetStencilWrite( U8 mask );

        static void             EnableDepthTest( Bool enable );
        static void             EnableStencilTest( Bool enable );
        static void             SetDepthFunc( Function f );
        static void             SetStencilOp( Operation stencilFail, Operation depthFail, Operation depthPass );
        static void             SetStencilFunc( Function f, U8 ref, U8 mask );

        static void             EnableCullFace( Bool enable );
        static void             SetCullFace( CullFace face );

        static void             EnableBlend( Bool enable );
        static void             SetBlendColor( F32 r, F32 g, F32 b, F32 a );
        static void             SetBlendFunc( BlendFunction src, BlendFunction dst );
        static void             SetBlendFuncSeparate( BlendFunction srcRGB, BlendFunction dstRGB, BlendFunction srcAlpha, BlendFunction dstAlpha );
        static void             SetBlendMode( BlendMode mode );

        static void             SetSRGBWrite( Bool enable );
        static void             SetViewport( SizeT x, SizeT y, SizeT width, SizeT height );

        static void             CatchError();

    protected:
        static void             ClearCache();
    };
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Graphic/ps/win32/RenderDevice.inl"
#else
    #error RenderDevice not implemented !
#endif

#endif // _GRAPHIC_RENDERDEVICE_H

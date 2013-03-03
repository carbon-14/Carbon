#include "Graphic/RenderCache.h"

#include "Graphic/RenderDevice.h"

namespace Graphic
{
    RenderCache::RenderCache()
        : m_clearDepth( 1.0f )
        , m_clearStencil( 0 )
        , m_sRGBWrite( false )
    {
        m_clearColor[0] = 0.0f;
        m_clearColor[1] = 0.0f;
        m_clearColor[2] = 0.0f;
        m_clearColor[3] = 0.0f;
    }

    RenderCache::~RenderCache()
    {
    }

    void RenderCache::Clear()
    {
        SetClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        SetClearDepth( 1.0f );
        SetClearStencil( 0 );
        SetSRGBWrite( false );

        SetRenderState( RenderState() );
    }

    void RenderCache::SetClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        if (
            r != m_clearColor[0] ||
            g != m_clearColor[1] ||
            b != m_clearColor[2] ||
            a != m_clearColor[3] )
        {
            RenderDevice::SetClearColor( r, g, b, a );
            m_clearColor[0] = r;
            m_clearColor[1] = g;
            m_clearColor[2] = b;
            m_clearColor[3] = a;
        }
    }

    void RenderCache::SetClearDepth( F32 depth )
    {
        if ( depth != m_clearDepth )
        {
            RenderDevice::SetClearDepth( depth );
            m_clearDepth = depth;
        }
    }

    void RenderCache::SetClearStencil( U8 stencil )
    {
        if ( stencil != m_clearStencil )
        {
            RenderDevice::SetClearStencil( stencil );
            m_clearStencil = stencil;
        }
    }

    void RenderCache::SetSRGBWrite( Bool enable )
    {
        if ( m_sRGBWrite != enable )
        {
            RenderDevice::SetSRGBWrite( enable );
            m_sRGBWrite = enable;
        }
    }

    void RenderCache::SetRenderState( const RenderState& renderState )
    {
        if ( renderState.m_bits0 != m_renderState.m_bits0 )
        {
            if ( renderState.m_stencilRef != m_renderState.m_stencilRef || renderState.m_stencilMask != m_renderState.m_stencilMask || renderState.m_stencilFunc != m_renderState.m_stencilFunc )
            {
                RenderDevice::SetStencilFunc( (Function)renderState.m_stencilFunc, renderState.m_stencilRef, renderState.m_stencilMask );
                m_renderState.m_stencilFunc = renderState.m_stencilFunc;
                m_renderState.m_stencilRef = renderState.m_stencilRef;
                m_renderState.m_stencilMask = renderState.m_stencilMask;
            }
            if ( renderState.m_stencilFail != m_renderState.m_stencilFail || renderState.m_depthFail != m_renderState.m_depthFail || renderState.m_depthPass != m_renderState.m_depthPass )
            {
                RenderDevice::SetStencilOp( (Operation)renderState.m_stencilFail, (Operation)renderState.m_depthFail, (Operation)renderState.m_depthPass );
                m_renderState.m_stencilFail = renderState.m_stencilFail;
                m_renderState.m_depthFail = renderState.m_depthFail;
                m_renderState.m_depthPass = renderState.m_depthPass;
            }
            if ( renderState.m_depthWriteMask != m_renderState.m_depthWriteMask )
            {
                RenderDevice::SetDepthWrite( renderState.m_depthWriteMask );
                m_renderState.m_depthWriteMask = renderState.m_depthWriteMask;
            }
            if ( renderState.m_enableDepthTest != m_renderState.m_enableDepthTest )
            {
                RenderDevice::EnableDepthTest( renderState.m_enableDepthTest );
                m_renderState.m_enableDepthTest = renderState.m_enableDepthTest;
            }
            if ( renderState.m_enableStencilTest != m_renderState.m_enableStencilTest )
            {
                RenderDevice::EnableStencilTest( renderState.m_enableStencilTest );
                m_renderState.m_enableStencilTest = renderState.m_enableStencilTest;
            }
            if ( renderState.m_enableCullFace != m_renderState.m_enableCullFace )
            {
                RenderDevice::EnableCullFace( renderState.m_enableCullFace );
                m_renderState.m_enableCullFace = renderState.m_enableCullFace;
            }
            if ( renderState.m_enableBlend != m_renderState.m_enableBlend )
            {
                RenderDevice::EnableBlend( renderState.m_enableBlend );
                m_renderState.m_enableBlend = renderState.m_enableBlend;
            }
            if ( renderState.m_cullFace != m_renderState.m_cullFace )
            {
                RenderDevice::SetCullFace( (CullFace)renderState.m_cullFace );
                m_renderState.m_cullFace = renderState.m_cullFace;
            }
            if ( renderState.m_srcAlphaBlendFunc != m_renderState.m_srcAlphaBlendFunc || renderState.m_dstAlphaBlendFunc != m_renderState.m_dstAlphaBlendFunc )
            {
                RenderDevice::SetBlendFuncSeparate( (BlendFunction)renderState.m_srcBlendFunc, (BlendFunction)renderState.m_dstBlendFunc, (BlendFunction)renderState.m_srcAlphaBlendFunc, (BlendFunction)renderState.m_dstAlphaBlendFunc );
                m_renderState.m_srcBlendFunc = renderState.m_srcBlendFunc;
                m_renderState.m_dstBlendFunc = renderState.m_dstBlendFunc;
                m_renderState.m_srcAlphaBlendFunc = renderState.m_srcAlphaBlendFunc;
                m_renderState.m_dstAlphaBlendFunc = renderState.m_dstAlphaBlendFunc;
            }
            else if ( renderState.m_srcBlendFunc != m_renderState.m_srcBlendFunc || renderState.m_dstBlendFunc != m_renderState.m_dstBlendFunc )
            {
                RenderDevice::SetBlendFunc( (BlendFunction)renderState.m_srcBlendFunc, (BlendFunction)renderState.m_dstBlendFunc );
                m_renderState.m_srcBlendFunc = renderState.m_srcBlendFunc;
                m_renderState.m_dstBlendFunc = renderState.m_dstBlendFunc;
            }
            if ( renderState.m_blendMode != m_renderState.m_blendMode )
            {
                RenderDevice::SetBlendMode( (BlendMode)renderState.m_blendMode );
                m_renderState.m_blendMode = renderState.m_blendMode;
            }
        }

        if ( renderState.m_bits1 != m_renderState.m_bits1 )
        {
            if ( renderState.m_stencilWriteMask != m_renderState.m_stencilWriteMask )
            {
                RenderDevice::SetStencilWrite( renderState.m_stencilWriteMask );
                m_renderState.m_stencilWriteMask = renderState.m_stencilWriteMask;
            }
            if ( renderState.m_colorWriteMask != m_renderState.m_colorWriteMask )
            {
                RenderDevice::SetColorWrite( renderState.m_colorWriteMask );
                m_renderState.m_colorWriteMask = renderState.m_colorWriteMask;
            }
            if ( renderState.m_depthFunc != m_renderState.m_depthFunc )
            {
                RenderDevice::SetDepthFunc( (Function)renderState.m_depthFunc );
                m_renderState.m_depthFunc = renderState.m_depthFunc;
            }
        }
    }
}

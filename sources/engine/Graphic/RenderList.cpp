#include "Graphic/RenderList.h"

#include "Graphic/RenderGeometry.h"

namespace Graphic
{
    RenderList::RenderList()
        : m_clearMask( 0 )
        , m_clearDepth( 1.0f )
        , m_clearStencil( 0 )
        , m_sRGBWrite( false )
    {
        m_clearColor[0] = 0.0f;
        m_clearColor[1] = 0.0f;
        m_clearColor[2] = 0.0f;
        m_clearColor[3] = 0.0f;
    }

    RenderList::~RenderList()
    {
        CARBON_ASSERT( m_list.Empty() );
    }

    void RenderList::Push( const RenderElement& element )
    {
        m_list.PushBack( element );
    }

    void RenderList::SetClearMask( U32 mask )
    {
        m_clearMask = mask;
    }

    void RenderList::SetClearColor( F32 r, F32 g, F32 b, F32 a )
    {
        m_clearColor[0] = r;
        m_clearColor[1] = g;
        m_clearColor[2] = b;
        m_clearColor[3] = a;
    }

    void RenderList::SetClearDepth( F32 depth )
    {
        m_clearDepth = depth;
    }

    void RenderList::SetClearStencil( U8 stencil )
    {
        m_clearStencil = stencil;
    }

    void RenderList::SetSRGBWrite( Bool enable )
    {
        m_sRGBWrite = enable;
    }

    void RenderList::Draw( RenderCache& renderCache )
    {
        // BEGIN
        if ( m_clearMask )
        {
            if ( m_clearMask & CM_COLOR )
                renderCache.SetClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
            if ( m_clearMask & CM_DEPTH )
                renderCache.SetClearDepth( m_clearDepth );
            if ( m_clearMask & CM_STENCIL )
                renderCache.SetClearStencil( m_clearStencil );
            RenderDevice::Clear( m_clearMask );
        }
        renderCache.SetSRGBWrite( m_sRGBWrite );

        // DRAW
        Core::Array< RenderElement >::Iterator it = m_list.Begin();
        Core::Array< RenderElement >::Iterator end = m_list.End();
        for ( ; it != end; ++it )
        {
            const RenderElement& e = *it;

            renderCache.SetProgram( e.m_program );
            renderCache.SetRenderState( e.m_renderState );
            renderCache.SetTextureUnits( e.m_textureUnits, e.m_textureUnitCount );
            renderCache.SetUniformBuffers( e.m_uniformBuffers, e.m_uniformBufferCount );

            e.m_geometry->Draw();
        }

        // END
        Clear();
    }

    void RenderList::Clear()
    {
        m_list.Reserve(0);
    }
}

#include "Graphic/RenderList.h"

#include "Graphic/RenderDevice.h"
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

    void RenderList::SetRenderState( const RenderState& state )
    {
        m_renderState = state;
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

    void RenderList::Draw( RenderCache& renderCache ) const
    {
        // BEGIN
        renderCache.SetSRGBWrite( m_sRGBWrite );
        renderCache.SetRenderState( m_renderState );

        if ( m_clearMask )
        {
            if ( m_clearMask & RM_COLOR )
                renderCache.SetClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
            if ( m_clearMask & RM_DEPTH )
                renderCache.SetClearDepth( m_clearDepth );
            if ( m_clearMask & RM_STENCIL )
                renderCache.SetClearStencil( m_clearStencil );
            RenderDevice::Clear( m_clearMask );
        }

        // DRAW
        Array< RenderElement >::ConstIterator it = m_list.Begin();
        Array< RenderElement >::ConstIterator end = m_list.End();
        for ( ; it != end; ++it )
        {
            const RenderElement& e = *it;

            ProgramCache::UseProgram( e.m_program );
            for ( SizeT i=0; i<e.m_textureCount; ++i )
            {
                const LayoutHandle& texture = e.m_textures[i];
                RenderDevice::BindTexture( texture.m_handle, texture.m_index );
            }
            for ( SizeT i=0; i<e.m_uniformBufferCount; ++i )
            {
                const LayoutHandle& uniformBuffer = e.m_uniformBuffers[i];
                RenderDevice::BindUniformBuffer( uniformBuffer.m_handle, uniformBuffer.m_index );
            }

            e.m_geometry->Draw();
        }

        // END
    }

    void RenderList::Clear()
    {
        m_list.Reserve(0);
    }
}

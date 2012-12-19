#include "Graphic/RenderList.h"

#include "Graphic/RenderGeometry.h"

namespace Graphic
{
    RenderList::RenderList()
        : m_sRGBWrite( false )
    {
    }

    RenderList::~RenderList()
    {
        CARBON_ASSERT( m_list.Empty() );
    }

    void RenderList::Push( const RenderElement& element )
    {
        m_list.PushBack( element );
    }

    void RenderList::SetSRGBWrite( Bool enable )
    {
        m_sRGBWrite = enable;
    }

    void RenderList::Draw( RenderCache& renderCache )
    {
        // BEGIN
        RenderDevice::SetSRGBWrite( m_sRGBWrite );

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

        RenderDevice::SetSRGBWrite( !m_sRGBWrite );
    }

    void RenderList::Clear()
    {
        m_list.Reserve(0);
    }
}

#include "Graphic/RenderList.h"

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

    void RenderList::Draw( const ProgramCache& programCache )
    {
        // BEGIN
        RenderDevice::SetSRGBWrite( m_sRGBWrite );

        // DRAW
        Core::Array< RenderElement >::Iterator it = m_list.Begin();
        Core::Array< RenderElement >::Iterator end = m_list.End();
        for ( ; it != end; ++it )
        {
            RenderElement& e = *it;

            programCache.UseProgram( e.m_program );

            for ( SizeT i=0; i<e.m_unitCount; ++i )
            {
                RenderDevice::SampleTexture( e.m_textures[ i ], e.m_samplers[ i ], i );
            }

            for ( SizeT i=0; i<e.m_geom.m_subGeomCount; ++i )
            {
                const SubGeometry& sub = e.m_geom.m_subGeoms[ i ];
                RenderDevice::BeginGeometry( e.m_geom.m_vertexDecl, sub.m_vertexArray, sub.m_indexBuffer );
                RenderDevice::DrawIndexed( e.m_primitive, sub.m_indexCount, e.m_geom.m_indexType );
                RenderDevice::EndGeometry( e.m_geom.m_vertexDecl );
            }

            for ( SizeT i=0; i<e.m_unitCount; ++i )
            {
                RenderDevice::SampleTexture( 0, 0, i );
            }
        }

        // END
        m_list.Clear();
        RenderDevice::UseProgram( 0 );
        RenderDevice::SetSRGBWrite( !m_sRGBWrite );
    }

    void RenderList::Clear()
    {
        m_list.Clear();
    }
}

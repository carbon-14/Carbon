#include "Graphic/RenderList.h"

namespace Graphic
{
    RenderList::RenderList()
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

    void RenderList::Draw( const ProgramCache& programCache )
    {
        // BEGIN

        // DRAW
        Core::Array< RenderElement >::Iterator it = m_list.Begin();
        Core::Array< RenderElement >::Iterator end = m_list.End();
        for ( ; it != end; ++it )
        {
            RenderElement& e = *it;

            programCache.UseProgram( e.m_program );
            RenderDevice::Draw( e.m_primitive, e.m_vertexArray );
        }

        // END
        m_list.Clear();
        RenderDevice::UseProgram( 0 );
    }

    void RenderList::Clear()
    {
        m_list.Clear();
    }
}

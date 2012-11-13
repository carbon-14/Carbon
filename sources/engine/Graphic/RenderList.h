#pragma once
#ifndef _GRAPHIC_RENDERLIST_H
#define _GRAPHIC_RENDERLIST_H

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"

#include "Core/Array.h"

namespace Graphic
{
    struct RenderElement
    {
        PrimitiveType   m_primitive;
        ProgramHandle   m_program;
        VertexArray *   m_vertexArray;
    };

    class _GraphicExport RenderList
    {
    public:
        RenderList();
        ~RenderList();

        void Push( const RenderElement& element );

        template < typename Pred >
        void Sort();
        void Draw( const ProgramCache& programCache );
        void Clear();

    private:
        Core::Array< RenderElement > m_list;
    };

    template< typename Pred >
    void RenderList::Sort()
    {
        // TODO
    }
}

template< > struct Core::IsPOD< Graphic::RenderElement > : public TrueType { };

#endif // _GRAPHIC_RENDERLIST_H

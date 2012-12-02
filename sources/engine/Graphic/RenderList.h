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

        Handle *        m_samplers;
        Handle *        m_textures;
        SizeT           m_unitCount;
    };

    class _GraphicExport RenderList
    {
    public:
        RenderList();
        ~RenderList();

        void Push( const RenderElement& element );

        void SetSRGBWrite( Bool enable );

        template < typename Pred >
        void Sort();
        void Draw( const ProgramCache& programCache );
        void Clear();

    private:
        Core::Array< RenderElement >    m_list;
        Bool                            m_sRGBWrite;
    };

    template< typename Pred >
    void RenderList::Sort()
    {
        // TODO
    }
}

CARBON_DECLARE_POD_TYPE( Graphic::RenderElement );

#endif // _GRAPHIC_RENDERLIST_H

#pragma once
#ifndef _GRAPHIC_RENDERLIST_H
#define _GRAPHIC_RENDERLIST_H

#include "Graphic/RenderCache.h"

#include "Core/Array.h"

namespace Graphic
{
    class RenderGeometry;

    struct RenderElement
    {
        ProgramHandle       m_program;
        RenderGeometry *    m_geometry;
        TextureUnit         m_textureUnits[ RenderDevice::ms_maxTextureUnitCount ];
        SizeT               m_textureUnitCount;
        Handle              m_uniformBuffers[ RenderDevice::ms_maxUniformBufferCount ];
        SizeT               m_uniformBufferCount;
    };

    class _GraphicExport RenderList
    {
    public:
        RenderList();
        ~RenderList();

        void Push( const RenderElement& element );

        void SetRenderState( const RenderState& state );
        void SetClearMask( U32 mask );
        void SetClearColor( F32 r, F32 g, F32 b, F32 a );
        void SetClearDepth( F32 depth );
        void SetClearStencil( U8 stencil );
        void SetSRGBWrite( Bool enable );

        template < typename Pred >
        void Sort();
        void Draw( RenderCache& renderCache );
        void Clear();

    private:
        Core::Array< RenderElement, Core::FrameAllocator >  m_list;

        RenderState m_renderState;
        U32         m_clearMask;
        F32         m_clearColor[4];
        F32         m_clearDepth;
        U8          m_clearStencil;
        Bool        m_sRGBWrite;
    };

    template< typename Pred >
    void RenderList::Sort()
    {
        // TODO
    }
}

CARBON_DECLARE_POD_TYPE( Graphic::RenderElement );

#endif // _GRAPHIC_RENDERLIST_H

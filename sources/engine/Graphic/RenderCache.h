#pragma once
#ifndef _GRAPHIC_RENDERCACHE_H
#define _GRAPHIC_RENDERCACHE_H

#include "Graphic/ProgramCache.h"
#include "Graphic/RenderState.h"

namespace Graphic
{
    class _GraphicExport RenderCache
    {
    public:
        RenderCache();
        ~RenderCache();

        void Clear();

        void SetClearColor( F32 r, F32 g, F32 b, F32 a );
        void SetClearDepth( F32 depth );
        void SetClearStencil( U8 stencil );
        void SetSRGBWrite( Bool enable );
        void SetRenderState( const RenderState& renderState );

    private:
        F32             m_clearColor[4];
        F32             m_clearDepth;
        U8              m_clearStencil;
        Bool            m_sRGBWrite;
        RenderState     m_renderState;
    };
}

#endif // _GRAPHIC_RENDERCACHE_H

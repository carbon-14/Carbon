#pragma once
#ifndef _GRAPHIC_LIGHTRENDERER_H
#define _GRAPHIC_LIGHTRENDERER_H

#include "Graphic/RenderState.h"
#include "Graphic/ProgramCache.h"

namespace Graphic
{
    class RenderCache;

    class _GraphicExport LightRenderer
    {
    public:
        LightRenderer();

        void Initialize();
        void Destroy();

        void Render( RenderCache& renderCache, Handle depthStencil, Handle normal, Handle color ) const;

    private:
        Handle          m_vertexArray;
        Handle          m_vertexBuffer;
        Handle          m_indexBuffer;
        ProgramHandle   m_program;
        RenderState     m_renderState;
    };
}

#endif // _GRAPHIC_LIGHTRENDERER

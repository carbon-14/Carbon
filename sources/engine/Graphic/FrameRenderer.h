#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/DLL.h"

#include "Core/Matrix.h"

#include "Graphic/RenderList.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"

namespace Graphic
{
    class Scene;

    class _GraphicExport FrameRenderer
    {
    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize();
        void Destroy();

        void Render( const Scene * scene, SizeT width, SizeT height );

    private:
        RenderCache     m_renderCache;
        RenderList      m_opaqueList;

        MeshRenderer    m_meshRenderer;
        LightRenderer   m_lightRenderer;
    };
}

#endif // _GRAPHIC_FRAMERENDERER_H

#pragma once
#ifndef _GRAPHIC_MESHRENDERER_H
#define _GRAPHIC_MESHRENDERER_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

namespace Graphic
{
    class RenderList;
    class Mesh;
    class DebugRenderer;

    class _GraphicExport MeshRenderer
    {
    public:
        void Initialize( DebugRenderer * debugRenderer );
        void Destroy();

        void Render( const Mesh * mesh, RenderList * opaqueList, Handle frameParameters ) const;

    private:
        DebugRenderer * m_debugRenderer;
    };
}

#endif // _GRAPHIC_MESHRENDERER_H

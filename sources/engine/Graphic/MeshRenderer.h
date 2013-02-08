#pragma once
#ifndef _GRAPHIC_MESHRENDERER_H
#define _GRAPHIC_MESHRENDERER_H

#include "Graphic/DLL.h"

namespace Graphic
{
    class RenderList;
    class Mesh;

    class _GraphicExport MeshRenderer
    {
    public:
        MeshRenderer();

        void Initialize( RenderList * opaqueList );
        void Destroy();

        void Render( const Mesh * mesh ) const;

    private:
        RenderList *    m_opaqueList;
    };
}

#endif // _GRAPHIC_MESHRENDERER_H

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
        struct Context
        {
            RenderList *    m_opaqueList;
        };

    public:
        void Initialize( DebugRenderer * debugRenderer );
        void Destroy();

        static Context * CreateContext( RenderList * opaqueList );
        static void DestroyContext( Context * context );

        void Render( const Mesh * const * meshes, SizeT meshCount, Context * context ) const;

    private:
        DebugRenderer * m_debugRenderer;
    };
}

#endif // _GRAPHIC_MESHRENDERER_H

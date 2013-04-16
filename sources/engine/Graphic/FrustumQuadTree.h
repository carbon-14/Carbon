#pragma once
#ifndef _GRAPHIC_FRUSTUMQUADTREE_H
#define _GRAPHIC_FRUSTUMQUADTREE_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

#include "Core/Vector.h"

namespace Graphic
{
    class Camera;

    class _GraphicExport FrustumQuadTree
    {
    public:
        struct Cell
        {
            Vector      hPlane;
            Vector      vPlane;
            U32         id;
        };

    public:
                        FrustumQuadTree();
                        ~FrustumQuadTree();

        void            Build( SizeT tileSize, SizeT width, SizeT height, const Camera * camera );
        void            Clear();

        void            Begin();
        void            End();

        SizeT           GetTileSize() const;
        SizeT           GetWidth() const;
        SizeT           GetHeight() const;
        const Camera *  GetCamera() const;
        SizeT           GetLevelCount() const;
        const Cell *    GetCells() const;

    private:
        SizeT           m_tileSize;
        SizeT           m_width;
        SizeT           m_height;
        const Camera *  m_camera;
        SizeT           m_levelCount;
        Cell *          m_cells;
        U16 *           m_resultCount;
        U16 *           m_results;
    };
}

#endif // _GRAPHIC_FRUSTUMQUADTREE_H

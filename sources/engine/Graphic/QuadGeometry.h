#pragma once
#ifndef _GRAPHIC_QUADGEOMETRY_H
#define _GRAPHIC_QUADGEOMETRY_H

#include "Graphic/Types.h"
#include "Graphic/RenderGeometry.h"

namespace Graphic
{
    class _GraphicExport QuadGeometry : RenderGeometry
    {
    public:
        static QuadGeometry& GetInstance();

        void Initialize();
        void Destroy();

        void Draw() const;

    private:
        QuadGeometry();

        static QuadGeometry m_instance;

        Handle  m_vertexArray;
        Handle  m_vertexBuffer;
    };
}

#endif // _GRAPHIC_QUADGEOMETRY_H

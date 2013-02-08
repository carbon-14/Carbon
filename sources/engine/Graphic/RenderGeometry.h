#pragma once
#ifndef _GRAPHIC_RENDERGEOMETRY_H
#define _GRAPHIC_RENDERGEOMETRY_H

#include "Graphic/DLL.h"

namespace Graphic
{
    class RenderCache;

    class _GraphicExport RenderGeometry
    {
    public:
        virtual void Draw() const = 0;
    };
}

#endif // _GRAPHIC_RENDERGEOMETRY_H

#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/DLL.h"

namespace Graphic
{
    class _GraphicExport FrameRenderer
    {
    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize();
        void Destroy();

        void Render();
    };
}

#endif // _GRAPHIC_FRAMERENDERER_H

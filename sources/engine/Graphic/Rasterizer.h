#pragma once
#ifndef _GRAPHIC_RASTERIZER_H
#define _GRAPHIC_RASTERIZER_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

#include "Core/Vector.h"

namespace Graphic
{
    class Camera;

    class _GraphicExport Rasterizer
    {
    public:
        struct Context
        {
            Vector *    m_vPlanes;
            Vector *    m_hPlanes;
            F32         m_near;
            F32         m_far;
            SizeT       m_vCount;
            SizeT       m_vSize;
            SizeT       m_hCount;
            SizeT       m_hSize;

            Handle      m_depthMinMaxTexture;
        };

    public:
        Rasterizer();
        ~Rasterizer();

        void Initialize();
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera );
        static void DestroyContext( Context * context );

        void Render( const Vector * const * spheres, SizeT sphereCount, Context * context ) const;
        void Draw( const Context ) const;

    private:
        static const SizeT ms_tileSize = 16;
};
}

#endif // _GRAPHIC_RASTERIZER_H

#pragma once
#ifndef _GRAPHIC_RASTERIZER_H
#define _GRAPHIC_RASTERIZER_H

#include "Graphic/ProgramCache.h"

#include "Core/Vector.h"

namespace Graphic
{
    class Camera;

    class _GraphicExport Rasterizer
    {
    public:
        struct Context
        {
            const Camera *          m_camera;
            Handle                  m_linearDepthTexture;
            SizeT                   m_width;
            SizeT                   m_height;
            SizeT                   m_size;

            Array< Vector >         m_vPlanes;
            Array< Vector >         m_hPlanes;

            Handle                  m_depthBuffer;
            Array< U8 >             m_mapCount;
            Array< const Vector * > m_map;

            Array< Vector >         m_spheres;
        };

    public:
        Rasterizer();
        ~Rasterizer();

        void Initialize();
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, Handle linearDepthTexture );
        static void DestroyContext( Context * context );

        void Render( const Vector * const * spheres, SizeT sphereCount, Context * context ) const;
        void Draw( const Context * context ) const;

    private:
        static const SizeT  ms_tileSize = 16;
        static const SizeT  ms_maxCount = 64;

        void FillQuad( const Vector * quad[4], const Vector * sphere, Context * context ) const;

        ProgramHandle       m_programTileDepth;
};
}

#endif // _GRAPHIC_RASTERIZER_H

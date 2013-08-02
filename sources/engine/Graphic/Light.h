#pragma once
#ifndef _GRAPHIC_LIGHT_H
#define _GRAPHIC_LIGHT_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"
#include "Graphic/ProgramCache.h"

#include "Core/Vector.h"

namespace Graphic
{
    enum LightType
    {
        LT_DIRECTIONAL,
        LT_OMNI,
        LT_SPOT,
        LT_COUNT
    };

    class _GraphicExport Light
    {
    public:
        void Update();

        Vector      m_value;
        Vector      m_position;
        Vector      m_orientation;
        F32         m_radius;
        F32         m_directionalWidth;
        F32         m_directionalHeight;
        F32         m_spotInAngle;
        F32         m_spotOutAngle;
        LightType   m_type;

        Vector GetBoundingSphere() const;

    private:
        Vector      m_boundingSphere;
    };

    class RenderGeometry;

    /*struct RenderLight
    {
        ProgramHandle           m_program;
        const RenderGeometry *  m_geometry;
        Handle                  m_uniformBuffer;
        Bool                    m_useFrontFace;
    };*/
}

CARBON_DECLARE_POD_TYPE( Graphic::Light );
//CARBON_DECLARE_POD_TYPE( Graphic::RenderLight );

#endif // _GRAPHIC_LIGHT_H

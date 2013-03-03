#pragma once
#ifndef _GRAPHIC_LIGHT_H
#define _GRAPHIC_LIGHT_H

#include "Graphic/DLL.h"
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

    struct _GraphicExport Light
    {
        Vector      m_value;
        Vector      m_position;
        Vector      m_orientation;
        F32         m_radius;
        F32         m_directionalWidth;
        F32         m_directionalHeight;
        F32         m_spotInAngle;
        F32         m_spotOutAngle;
        LightType   m_type;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::Light );

#endif // _GRAPHIC_LIGHT_H

#pragma once
#ifndef _GRAPHIC_RENDERSTATE_H
#define _GRAPHIC_RENDERSTATE_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

#include "Core/TypeTraits.h"

namespace Graphic
{
    class _GraphicExport RenderState
    {
    public:
        RenderState();

        union
        {
            struct
            {
                U32 m_stencilRef            : 8;    // 8
                U32 m_stencilMask           : 8;    // 16
                U32 m_stencilFunc           : 4;    // 20
                U32 m_stencilFail           : 4;    // 24
                U32 m_depthFail             : 4;    // 28
                U32 m_depthPass             : 4;    // 32
                U32 m_depthWriteMask        : 1;    // 33
                U32 m_enableDepthTest       : 1;    // 34
                U32 m_enableStencilTest     : 1;    // 35
                U32 m_enableCullFace        : 1;    // 36
                U32 m_enableBlend           : 1;    // 37
                U32 m_cullFace              : 1;    // 33
                U32 m_srcBlendFunc          : 5;    // 43
                U32 m_dstBlendFunc          : 5;    // 48
                U32 m_srcAlphaBlendFunc     : 5;    // 53
                U32 m_dstAlphaBlendFunc     : 5;    // 58
                U32 m_blendMode             : 3;    // 61
                
                U32 m_padding0              : 3;

                U32 m_stencilWriteMask      : 8;    // 8
                U32 m_colorWriteMask        : 4;    // 12
                U32 m_depthFunc             : 4;    // 16

                U32 m_padding1              : 16;
            };
            struct
            {
                U64 m_bits0;    // most used states
                U32 m_bits1;    // less used states
            };
        };
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::RenderState );

#endif // _GRAPHIC_RENDERSTATE_H

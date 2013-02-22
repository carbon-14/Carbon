#include "RenderState.h"

namespace Graphic
{
    RenderState::RenderState()
        : m_stencilRef          ( 0 )
        , m_stencilMask         ( 0xFF )
        , m_stencilFunc         ( F_ALWAYS )
        , m_stencilFail         ( O_KEEP )
        , m_depthFail           ( O_KEEP )
        , m_depthPass           ( O_KEEP )
        , m_depthWriteMask      ( 1 )
        , m_enableDepthTest     ( 1 )
        , m_enableCullFace      ( 1 )
        , m_cullFace            ( CF_BACK )        
        , m_srcBlendFunc        ( BF_ONE )
        , m_dstBlendFunc        ( BF_ZERO )
        , m_srcAlphaBlendFunc   ( BF_ONE )
        , m_dstAlphaBlendFunc   ( BF_ZERO )
        , m_blendMode           ( BM_ADD )

        , m_padding0            ( 0 )

        
        , m_stencilWriteMask    ( 0xFF )
        , m_colorWriteMask      ( CM_RGBA )
        , m_depthFunc           ( F_LEQUAL )

        , m_padding1            ( 0 )
    {
    }
};

#include "Graphic/LightRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderCache.h"

namespace Graphic
{
    LightRenderer::LightRenderer()
        : m_vertexArray( 0 )
        , m_vertexBuffer( 0 )
        , m_indexBuffer( 0 )
    {
    }

    void LightRenderer::Initialize()
    {
        const F32 vb[4][4] =
        {
            { -1.0f, -1.0f, 0.0f, 0.0f },
            { +1.0f, -1.0f, 1.0f, 0.0f },
            { -1.0f, +1.0f, 0.0f, 1.0f },
            { +1.0f, +1.0f, 1.0f, 1.0f }
        };

        const U8 ib[2][3] =
        {
            0, 1, 2,
            2, 1, 3,
        };

        VertexDeclaration vdecl;
        vdecl.m_attributes[0].m_semantic    = VS_POSITION;
        vdecl.m_attributes[0].m_type        = DT_F32;
        vdecl.m_attributes[0].m_size        = 4;
        vdecl.m_attributes[0].m_normalized  = false;
        vdecl.m_attributes[0].m_offset      = 0;
        vdecl.m_size                        = 16;
        vdecl.m_count                       = 1;

        m_vertexBuffer  = RenderDevice::CreateVertexBuffer( sizeof(vb), vb, BU_STATIC );
        m_indexBuffer   = RenderDevice::CreateIndexBuffer( sizeof(ib), ib, BU_STATIC );
        m_vertexArray   = RenderDevice::CreateVertexArray( vdecl, m_vertexBuffer, m_indexBuffer );

        const U32 programId = ProgramCache::CreateId( "lighting" );
        m_program           = ProgramCache::GetProgram( programId );

        m_renderState.m_enableDepthTest = false;
        m_renderState.m_depthWriteMask = false;
    }

    void LightRenderer::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyBuffer( m_vertexBuffer );
    }

    void LightRenderer::Render( RenderCache& renderCache, Handle depthStencil, Handle normal, Handle color ) const
    {
        renderCache.SetSRGBWrite( true );
        renderCache.SetRenderState( m_renderState );

        ProgramCache::UseProgram( m_program );

        RenderDevice::BindTexture( depthStencil , 0 );
        RenderDevice::BindTexture( normal       , 1 );
        RenderDevice::BindTexture( color        , 2 );

        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::DrawIndexed( PT_TRIANGLES, 6, DT_U8 );
        RenderDevice::EndGeometry();
    }
}

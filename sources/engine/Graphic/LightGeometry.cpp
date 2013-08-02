#include "Graphic/LightGeometry.h"

#include "Graphic/RenderDevice.h"

namespace Graphic
{
    void DirectionalGeometry::Initialize()
    {
        const F32 vb[8][4] =
        {
            { -1.0f, -1.0f, +0.0f, +1.0f },
            { +1.0f, -1.0f, +0.0f, +1.0f },
            { -1.0f, +1.0f, +0.0f, +1.0f },
            { +1.0f, +1.0f, +0.0f, +1.0f },
            { -1.0f, -1.0f, -1.0f, +1.0f },
            { +1.0f, -1.0f, -1.0f, +1.0f },
            { -1.0f, +1.0f, -1.0f, +1.0f },
            { +1.0f, +1.0f, -1.0f, +1.0f },
        };

        const U8 ib[36] = 
        {
            0, 1, 2,    // Back
            2, 1, 3,
            1, 0, 5,    // Bottom
            5, 0, 4,
            3, 1, 7,    // Right
            7, 1, 5,
            2, 3, 6,    // Top
            6, 3, 7,
            0, 2, 4,    // Left
            4, 2, 6,
            4, 6, 5,    // Front
            5, 6, 7
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
    }

    void DirectionalGeometry::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyIndexBuffer( m_indexBuffer );
        RenderDevice::DestroyVertexBuffer( m_vertexBuffer );
    }

    void DirectionalGeometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::DrawIndexed( PT_TRIANGLES, 36, DT_U8 );
        RenderDevice::EndGeometry();
    }

    void OmniGeometry::Initialize()
    {
        const F32 vb[8][4] =
        {
            { -1.0f, -1.0f, +1.0f, +1.0f },
            { +1.0f, -1.0f, +1.0f, +1.0f },
            { -1.0f, +1.0f, +1.0f, +1.0f },
            { +1.0f, +1.0f, +1.0f, +1.0f },
            { -1.0f, -1.0f, -1.0f, +1.0f },
            { +1.0f, -1.0f, -1.0f, +1.0f },
            { -1.0f, +1.0f, -1.0f, +1.0f },
            { +1.0f, +1.0f, -1.0f, +1.0f },
        };

        const U8 ib[36] = 
        {
            0, 1, 2,    // Back
            2, 1, 3,
            1, 0, 5,    // Bottom
            5, 0, 4,
            3, 1, 7,    // Right
            7, 1, 5,
            2, 3, 6,    // Top
            6, 3, 7,
            0, 2, 4,    // Left
            4, 2, 6,
            4, 6, 5,    // Front
            5, 6, 7
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
    }

    void OmniGeometry::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyIndexBuffer( m_indexBuffer );
        RenderDevice::DestroyVertexBuffer( m_vertexBuffer );
    }

    void OmniGeometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::DrawIndexed( PT_TRIANGLES, 36, DT_U8 );
        RenderDevice::EndGeometry();
    }

    void SpotGeometry::Initialize()
    {
        const F32 vb[8][4] =
        {
            { +0.0f, +0.0f, +0.0f, +1.0f },
            { -1.0f, -1.0f, -1.0f, +1.0f },
            { +1.0f, -1.0f, -1.0f, +1.0f },
            { -1.0f, +1.0f, -1.0f, +1.0f },
            { +1.0f, +1.0f, -1.0f, +1.0f },
        };

        const U8 ib[18] = 
        {
            0, 1, 2,
            0, 2, 4,
            0, 4, 3,
            0, 3, 1,
            1, 3, 2,
            2, 3, 4
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
    }

    void SpotGeometry::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyIndexBuffer( m_indexBuffer );
        RenderDevice::DestroyVertexBuffer( m_vertexBuffer );
    }

    void SpotGeometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::DrawIndexed( PT_TRIANGLES, 18, DT_U8 );
        RenderDevice::EndGeometry();
    }
}

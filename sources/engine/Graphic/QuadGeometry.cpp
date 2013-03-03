#include "Graphic/QuadGeometry.h"

#include "graphic/RenderDevice.h"

namespace Graphic
{
    QuadGeometry QuadGeometry::m_instance;

    QuadGeometry::QuadGeometry()
    {
    }

    QuadGeometry& QuadGeometry::GetInstance()
    {
        return m_instance;
    }

    void QuadGeometry::Initialize()
    {
        const F32 vb[6][4] =
        {
            { -1.0f, -1.0f, 0.0f, 0.0f },
            { +1.0f, -1.0f, 1.0f, 0.0f },
            { -1.0f, +1.0f, 0.0f, 1.0f },
            { -1.0f, +1.0f, 0.0f, 1.0f },
            { +1.0f, -1.0f, 1.0f, 0.0f },
            { +1.0f, +1.0f, 1.0f, 1.0f }
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
        m_vertexArray   = RenderDevice::CreateVertexArray( vdecl, m_vertexBuffer, 0 );
    }

    void QuadGeometry::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyBuffer( m_vertexBuffer );
    }

    void QuadGeometry::Draw() const
    {
        RenderDevice::BeginGeometry( m_vertexArray );
        RenderDevice::Draw( PT_TRIANGLES, 6 );
        RenderDevice::EndGeometry();
    }
}

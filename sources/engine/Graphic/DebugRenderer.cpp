#include "Graphic/DebugRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/RenderCache.h"

namespace Graphic
{
    VertexDeclaration DebugRenderer::CreateVertexDeclaration()
    {
        VertexDeclaration vdecl;
        vdecl.m_attributes[0].m_semantic    = VS_POSITION;
        vdecl.m_attributes[0].m_type        = DT_F32;
        vdecl.m_attributes[0].m_size        = 3;
        vdecl.m_attributes[0].m_normalized  = false;
        vdecl.m_attributes[0].m_offset      = 0;
        vdecl.m_attributes[1].m_semantic    = VS_COLOR;
        vdecl.m_attributes[1].m_type        = DT_U8;
        vdecl.m_attributes[1].m_size        = 4;
        vdecl.m_attributes[1].m_normalized  = true;
        vdecl.m_attributes[1].m_offset      = 12;
        vdecl.m_size                        = 16;
        vdecl.m_count                       = 2;
        return vdecl;
    }

    const VertexDeclaration DebugRenderer::m_vdecl = DebugRenderer::CreateVertexDeclaration();

    void DebugRenderer::Initialize()
    {
        m_vertices.Reserve( 128 );

        U32 programId   = ProgramCache::CreateId( "debugLine" );
        m_program       = ProgramCache::GetProgram( programId );

        //m_state.m_enableDepthTest = true;

        m_vertexBuffer      = 0;
        m_vertexBufferSize  = 0;
        m_vertexArray       = 0;
    }

    void DebugRenderer::Destroy()
    {
        RenderDevice::DestroyVertexArray( m_vertexArray );
        RenderDevice::DestroyBuffer( m_vertexBuffer );
    }

    void DebugRenderer::RenderLine( const Vector& position0, const Vector& position1, const Vector& color )
    {
        F128 tmp;
        Vertex p;

        Store( tmp, color * Splat( 255.0f ) );
        p.m_color[0] = (U8)tmp[0];
        p.m_color[1] = (U8)tmp[1];
        p.m_color[2] = (U8)tmp[2];
        p.m_color[3] = (U8)tmp[3];

        Store( tmp, position0 );
        MemoryUtils::MemCpy( p.m_pos, tmp, 12 );
        m_vertices.PushBack( p );

        Store( tmp, position1 );
        MemoryUtils::MemCpy( p.m_pos, tmp, 12 );
        m_vertices.PushBack( p );
    }

    void DebugRenderer::Draw( RenderCache& renderCache, Handle frameParameters )
    {
        if ( ! m_vertices.Empty() )
        {
            if ( m_vertexBufferSize < m_vertices.Capacity() )
            {
                RenderDevice::DestroyVertexArray( m_vertexArray );
                RenderDevice::DestroyBuffer( m_vertexBuffer );
                m_vertexBuffer = RenderDevice::CreateVertexBuffer( sizeof(Vertex) * m_vertices.Capacity(), 0, BU_DYNAMIC );
                m_vertexBufferSize = m_vertices.Capacity();
                m_vertexArray = RenderDevice::CreateVertexArray( m_vdecl, m_vertexBuffer );
            }

            void * data = RenderDevice::MapVertexBuffer( m_vertexBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, m_vertices.ConstPtr(), sizeof(Vertex) * m_vertices.Size() );
            RenderDevice::UnmapVertexBuffer();

            ProgramCache::UseProgram( m_program );
            renderCache.SetSRGBWrite( false );
            renderCache.SetRenderState( m_state );

            RenderDevice::BindUniformBuffer( frameParameters, LI_FRAME );

            RenderDevice::BeginGeometry( m_vertexArray );
            RenderDevice::Draw( PT_LINES, m_vertices.Size() );
            RenderDevice::EndGeometry();

            m_vertices.Clear();
        }
    }
}

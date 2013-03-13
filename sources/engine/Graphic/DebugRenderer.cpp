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
        U32 programId   = ProgramCache::CreateId( "debugLine" );
        m_program       = ProgramCache::GetProgram( programId );

        m_state.m_enableDepthTest = true;
    }

    void DebugRenderer::Destroy()
    {
    }

    DebugRenderer::Context * DebugRenderer::CreateContext()
    {
        Context * context = MemoryManager::New< Context >();

        context->m_vertices.Reserve(128);
        context->m_vertexBuffer = 0;
        context->m_vertexBufferSize = 0;
        context->m_vertexArray = 0;

        return context;
    }

    void DebugRenderer::UpdateContext( Context * context )
    {
        context->m_vertices.Clear();
    }

    void DebugRenderer::DestroyContext( Context * context )
    {
        RenderDevice::DestroyVertexArray( context->m_vertexArray );
        RenderDevice::DestroyBuffer( context->m_vertexBuffer );

        MemoryManager::Delete( context );
    }

    void DebugRenderer::Render( const DebugLine& line, Context * context ) const
    {
        F128 tmp;
        Vertex p;

        Store( tmp, line.m_color * Splat( 255.0f ) );
        p.m_color[0] = (U8)tmp[0];
        p.m_color[1] = (U8)tmp[1];
        p.m_color[2] = (U8)tmp[2];
        p.m_color[3] = (U8)tmp[3];

        Store( tmp, line.m_position0 );
        MemoryUtils::MemCpy( p.m_pos, tmp, 12 );
        context->m_vertices.PushBack( p );

        Store( tmp, line.m_position1 );
        MemoryUtils::MemCpy( p.m_pos, tmp, 12 );
        context->m_vertices.PushBack( p );
    }

    void DebugRenderer::Draw( Context * context, RenderCache& renderCache ) const
    {
        if ( ! context->m_vertices.Empty() )
        {
            if ( context->m_vertexBufferSize < context->m_vertices.Capacity() )
            {
                RenderDevice::DestroyVertexArray( context->m_vertexArray );
                RenderDevice::DestroyBuffer( context->m_vertexBuffer );
                context->m_vertexBuffer = RenderDevice::CreateVertexBuffer( sizeof(Vertex) * context->m_vertices.Capacity(), 0, BU_DYNAMIC );
                context->m_vertexBufferSize = context->m_vertices.Capacity();
                context->m_vertexArray = RenderDevice::CreateVertexArray( m_vdecl, context->m_vertexBuffer );
            }

            void * data = RenderDevice::MapVertexBuffer( context->m_vertexBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, context->m_vertices.ConstPtr(), sizeof(Vertex) * context->m_vertices.Size() );
            RenderDevice::UnmapVertexBuffer();

            ProgramCache::UseProgram( m_program );
            renderCache.SetSRGBWrite( false );
            renderCache.SetRenderState( m_state );

            RenderDevice::BeginGeometry( context->m_vertexArray );
            RenderDevice::Draw( PT_LINES, context->m_vertices.Size() );
            RenderDevice::EndGeometry();
        }
    }
}

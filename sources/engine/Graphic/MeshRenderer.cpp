#include "Graphic/MeshRenderer.h"

#include "Graphic/Mesh.h"

#include "Graphic/RenderGeometry.h"
#include "Graphic/RenderList.h"

namespace Graphic
{
    void MeshRenderer::Initialize( DebugRenderer * debugRenderer )
    {
        m_debugRenderer = debugRenderer;
    }

    void MeshRenderer::Destroy()
    {
        m_debugRenderer = 0;
    }

    MeshRenderer::Context * MeshRenderer::CreateContext( RenderList * opaqueList )
    {
        Context * context = MemoryManager::New< Context >();

        context->m_opaqueList = opaqueList;

        return context;
    }

    void MeshRenderer::DestroyContext( Context * context )
    {
        MemoryManager::Delete( context );
    }

    void MeshRenderer::Render( const Mesh * mesh, Context * context ) const
    {
        RenderElement e;

        LayoutHandle instanceParams = { mesh->GetUniformBuffer(), LI_INSTANCE };
        e.m_uniformBuffers[0]       = instanceParams;

        e.m_uniformBufferCount=1;

        const MeshResource * rsc = mesh->GetResource();
        for ( SizeT i=0; i<rsc->GetSubMeshCount(); ++i )
        {
            e.m_geometry = mesh->GetGeom(i);

            const MaterialResource * material = rsc->GetSubMeshes()[ i ].m_material.ConstPtr();
            e.m_program = material->GetProgram();
            
            for ( e.m_textureCount=0; e.m_textureCount<material->GetTextureCount(); ++e.m_textureCount )
            {
                const MaterialResource::Texture& texture    = material->GetTexture( e.m_textureCount );
                e.m_textures[ e.m_textureCount ].m_handle   = texture.m_resource->GetTexture();
                e.m_textures[ e.m_textureCount ].m_index    = texture.m_index;
            }

            context->m_opaqueList->Push( e );
        }
    }
}

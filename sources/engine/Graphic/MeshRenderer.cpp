#include "Graphic/MeshRenderer.h"

#include "Graphic/Mesh.h"

#include "Graphic/RenderGeometry.h"
#include "Graphic/RenderList.h"

namespace Graphic
{
    MeshRenderer::MeshRenderer()
        : m_opaqueList( 0 )
    {

    }

    void MeshRenderer::Initialize( RenderList * opaqueList )
    {
        CARBON_ASSERT( !m_opaqueList );
        m_opaqueList = opaqueList;
    }

    void MeshRenderer::Destroy()
    {
        m_opaqueList = 0;
    }

    void MeshRenderer::Render( const Mesh * mesh ) const
    {
        RenderElement e;

        const MeshResource * rsc = mesh->GetResource();

        for ( SizeT i=0; i<rsc->GetSubMeshCount(); ++i )
        {
            const MaterialResource * material = rsc->GetSubMeshes()[ i ].m_material.ConstPtr();
            e.m_geometry = mesh->GetGeoms() + i;
            e.m_program = material->GetProgram();
            
            for ( e.m_textureCount=0; e.m_textureCount<material->GetTextureCount(); ++e.m_textureCount )
            {
                const MaterialResource::Texture& texture = material->GetTexture( e.m_textureCount );
                e.m_textures[ e.m_textureCount ].m_handle = texture.m_resource->GetTexture();
                e.m_textures[ e.m_textureCount ].m_index = texture.m_index;
            }

            e.m_uniformBufferCount=0;

            m_opaqueList->Push( e );
        }
    }
}

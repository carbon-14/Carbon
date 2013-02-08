#include "UnitTest/Level7/Level7.h"

#include "Core/ResourceManager.h"

#include "Graphic/RenderState.h"

using namespace Graphic;

namespace Level7_NS
{
}

using namespace Level7_NS;

Level7::Level7( const RenderWindow& window )
    : Application( window )
{
}

void Level7::ProcessInputs( RAWINPUT * raw )
{
}

void Level7::PreExecute()
{
    RenderState opaque;
    opaque.m_enableDepthTest = true;
    opaque.m_enableCullFace  = true;

    m_renderList.SetRenderState( opaque );
    m_renderList.SetClearMask( CM_COLOR | CM_DEPTH );

    m_mesh = ResourceManager::Create< MeshResource >( "level7/sphere.bmh" );
}

void Level7::PostExecute()
{
    m_mesh = 0;

    m_renderCache.Clear();
}

void Level7::Execute()
{
    /*{
        RenderElement element;

        element.m_textureCount  = 0;
        element.m_uniformBufferCount = 0;

        for ( SizeT i=0; i<m_mesh->GetSubMeshCount(); ++i )
        {
            const MaterialResource * material = m_mesh->GetSubMeshes()[i].m_material.ConstPtr();
            element.m_program = material->GetProgram();
            for ( element.m_textureCount = 0; element.m_textureCount<material->GetTextureCount(); ++element.m_textureCount )
            {
                const MaterialResource::Texture& texture = material->GetTexture( element.m_textureCount );
                if ( texture.m_resource && texture.m_resource->IsReady() )
                {
                    element.m_textures[ element.m_textureCount ].m_handle   = texture.m_resource->GetTexture();
                }
                else
                {
                    element.m_textures[ element.m_textureCount ].m_handle   = 0;
                }
                element.m_textures[ element.m_textureCount ].m_index    = texture.m_index;
            }
            element.m_geometry = (RenderMesh*)MemoryManager::FrameAlloc( sizeof(RenderMesh), MemoryUtils::AlignOf< RenderMesh >() );
            ::new( element.m_geometry ) RenderMesh( m_mesh.Ptr(), i );

            renderList.Push( element );
        }
    }*/


    Handle depthStencil = RenderDevice::CreateRenderTarget( TF_D24S8, m_window.width, m_window.height );
    Handle albedo = RenderDevice::CreateRenderTarget( TF_RGBA8, m_window.width, m_window.height );

    Handle framebuffer = RenderDevice::CreateFramebuffer();

    RenderDevice::BindFramebuffer( framebuffer, FT_DRAW );
    RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, albedo, 0 );
    RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, depthStencil, 0 );

    m_renderList.Draw( m_renderCache );

    RenderDevice::BindFramebuffer( 0, FT_DRAW );

    RenderDevice::DestroyTexture( albedo );
    RenderDevice::DestroyTexture( depthStencil );
}


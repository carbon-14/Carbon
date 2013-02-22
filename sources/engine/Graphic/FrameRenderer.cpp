#include "Graphic/FrameRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"

namespace Graphic
{
    FrameRenderer::FrameRenderer()
    {

    }

    FrameRenderer::~FrameRenderer()
    {

    }

    void FrameRenderer::Initialize()
    {
        m_meshRenderer.Initialize( &m_opaqueList );
        m_lightRenderer.Initialize();
        m_renderCache.Clear();

        RenderState opaque;
        opaque.m_enableDepthTest = true;
        opaque.m_enableCullFace  = true;

        m_opaqueList.SetRenderState( opaque );
        m_opaqueList.SetClearMask( CM_COLOR | CM_DEPTH );
    }

    void FrameRenderer::Destroy()
    {
        m_meshRenderer.Destroy();
        m_lightRenderer.Destroy();
        m_renderCache.Clear();
    }

    void FrameRenderer::Render( const Scene * scene, SizeT width, SizeT height )
    {
        // Render meshes
        {
            Scene::MeshArray::ConstIterator it  = scene->GetMeshes().Begin();
            Scene::MeshArray::ConstIterator end = scene->GetMeshes().End();
            for ( ; it!=end; ++it )
            {
                m_meshRenderer.Render( *it, scene->GetUniformBuffer() );
            }
        }

        Handle depthStencil = RenderDevice::CreateRenderTarget( TF_D24S8, width, height );
        Handle normal       = RenderDevice::CreateRenderTarget( TF_RG16, width, height );
        Handle color        = RenderDevice::CreateRenderTarget( TF_RGBA8, width, height );

        // Draw G-Buffer
        {
            Handle framebuffer  = RenderDevice::CreateFramebuffer();
            RenderDevice::BindFramebuffer( framebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, depthStencil, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, normal, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR1, color, 0 );

            m_opaqueList.Draw( m_renderCache );

            RenderDevice::BindFramebuffer( 0, FT_DRAW );
            RenderDevice::DestroyFramebuffer( framebuffer );
        }

        // Lighting
        {
            m_lightRenderer.Render( m_renderCache, depthStencil, normal, color );
        }

        RenderDevice::DestroyTexture( color );
        RenderDevice::DestroyTexture( normal );
        RenderDevice::DestroyTexture( depthStencil );

        m_renderCache.Clear();
    }
}

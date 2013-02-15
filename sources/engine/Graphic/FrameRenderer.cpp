#include "Graphic/FrameRenderer.h"

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
        m_renderCache.Clear();
    }

    void FrameRenderer::Render( const Scene * scene )
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

        m_opaqueList.Draw( m_renderCache );
        
        m_renderCache.Clear();
    }
}

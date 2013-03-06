#include "Graphic/FrameRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"

namespace Graphic
{
    struct FrameParameters
    {
        Vector  m_viewportSize;
        Vector  m_depthRange;
        Vector  m_viewScale;
        Vector  m_ambientSkyLight;
        Vector  m_ambientGroundLight;
        Matrix  m_viewMatrix;
        Matrix  m_projMatrix;
        Matrix  m_viewProjMatrix;
    };

    FrameRenderer::FrameRenderer()
    {

    }

    FrameRenderer::~FrameRenderer()
    {

    }

    void FrameRenderer::Initialize()
    {
        m_debugRenderer.Initialize();
        m_meshRenderer.Initialize( &m_opaqueList );
        m_lightRenderer.Initialize( &m_debugRenderer );

        m_renderCache.Clear();

        RenderState opaque;
        opaque.m_enableDepthTest = true;
        opaque.m_enableCullFace  = true;

        m_opaqueList.SetRenderState( opaque );
        m_opaqueList.SetClearMask( RM_ALL );

        const U32 programLinearDepthId  = ProgramCache::CreateId( "linearDepth" );
        m_programLinearDepth            = ProgramCache::GetProgram( programLinearDepthId );

        const U32 programToneMappingId  = ProgramCache::CreateId( "toneMapping" );
        m_programToneMapping            = ProgramCache::GetProgram( programToneMappingId );

        m_renderStateLinearDepth.m_depthWriteMask = false;
        m_renderStateToneMapping.m_depthWriteMask = false;

        m_frameUniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(FrameParameters), 0, BU_DYNAMIC );
    }

    void FrameRenderer::Destroy()
    {
        RenderDevice::DestroyBuffer( m_frameUniformBuffer );

        m_meshRenderer.Destroy();
        m_lightRenderer.Destroy();
        m_debugRenderer.Destroy();
        m_renderCache.Clear();
    }

    void FrameRenderer::Render( const FrameContext& context )
    {
        UpdateFrameUniformBuffer( context );

        // Render meshes
        {
            Scene::MeshArray::ConstIterator it  = context.m_scene->GetMeshes().Begin();
            Scene::MeshArray::ConstIterator end = context.m_scene->GetMeshes().End();
            for ( ; it!=end; ++it )
            {
                m_meshRenderer.Render( *it, m_frameUniformBuffer );
            }
        }

        // Render lights
        {
            m_lightRenderer.Render( context.m_scene, context.m_camera );
        }

        // Draw G-Buffer
        {
            RenderDevice::BindFramebuffer( context.m_geomFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, context.m_depthStencilTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context.m_normalTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR1, context.m_colorTexture, 0 );
            RenderDevice::DrawBuffer( CBB_COLOR0 | CBB_COLOR1 );

            m_opaqueList.Draw( m_renderCache );
        }

        // Linearize depth
        {
            RenderDevice::BindFramebuffer( context.m_linearizeDepthFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context.m_linearDepthTexture, 0 );

            LinearizeDepth( context.m_depthStencilTexture );
        }

        // Draw Lights
        {
            RenderDevice::BindFramebuffer( context.m_lightFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, context.m_depthStencilTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context.m_lightTexture, 0 );

            m_lightRenderer.Draw( m_renderCache, m_frameUniformBuffer, context.m_linearDepthTexture, context.m_normalTexture, context.m_colorTexture );
        }

        {
            RenderDevice::BindFramebuffer( context.m_finalFramebuffer, FT_BOTH );
            RenderDevice::AttachTexture( FT_BOTH, FA_DEPTH_STENCIL, context.m_depthStencilTexture, 0 );
            RenderDevice::AttachRenderbuffer( FT_BOTH, FA_COLOR0, context.m_finalColorBuffer );

            // Tone mapping
            ApplyToneMapping( context.m_lightTexture );

            // Debug
            m_debugRenderer.Draw( m_renderCache, m_frameUniformBuffer );

            // Blit
            RenderDevice::BindFramebuffer( 0, FT_DRAW );
            RenderDevice::DrawBuffer( CBB_BACK );
            RenderDevice::ReadBuffer( CB_COLOR0 );
            RenderDevice::BlitFramebuffer( 0, 0, context.m_width, context.m_height, 0, 0, context.m_width, context.m_height, RM_COLOR, FT_POINT );
        }
    }

    void FrameRenderer::InitializeFrameContext( FrameContext& context, SizeT width, SizeT height, Camera * camera, Scene * scene )
    {
        CARBON_ASSERT( width );
        CARBON_ASSERT( height );

        context.m_width                     = width;
        context.m_height                    = height;
        context.m_camera                    = camera;
        context.m_scene                     = scene;

        context.m_depthStencilTexture       = RenderDevice::CreateRenderTexture( TF_D24S8, width, height );
        context.m_normalTexture             = RenderDevice::CreateRenderTexture( TF_RG16F, width, height );
        context.m_colorTexture              = RenderDevice::CreateRenderTexture( TF_SRGBA8, width, height );
        context.m_linearDepthTexture        = RenderDevice::CreateRenderTexture( TF_R32F, width, height );
        context.m_lightTexture              = RenderDevice::CreateRenderTexture( TF_RGBA16F, width, height );
        context.m_finalColorBuffer          = RenderDevice::CreateRenderbuffer( TF_SRGBA8, width, height );

        context.m_geomFramebuffer           = RenderDevice::CreateFramebuffer();
        context.m_linearizeDepthFramebuffer = RenderDevice::CreateFramebuffer();
        context.m_lightFramebuffer          = RenderDevice::CreateFramebuffer();
        context.m_finalFramebuffer          = RenderDevice::CreateFramebuffer();

        RenderDevice::BindFramebuffer( 0, FT_BOTH );
    }

    void FrameRenderer::DestroyFrameContext( FrameContext& context )
    {
        RenderDevice::DestroyFramebuffer( context.m_finalFramebuffer );
        RenderDevice::DestroyFramebuffer( context.m_lightFramebuffer );
        RenderDevice::DestroyFramebuffer( context.m_linearizeDepthFramebuffer );
        RenderDevice::DestroyFramebuffer( context.m_geomFramebuffer );

        RenderDevice::DestroyRenderbuffer( context.m_finalColorBuffer );
        RenderDevice::DestroyTexture( context.m_lightTexture );
        RenderDevice::DestroyTexture( context.m_linearDepthTexture );
        RenderDevice::DestroyTexture( context.m_colorTexture );
        RenderDevice::DestroyTexture( context.m_normalTexture );
        RenderDevice::DestroyTexture( context.m_depthStencilTexture );

        Handle framebuffer = RenderDevice::CreateFramebuffer();
    }

    void FrameRenderer::SetLightDebugDraw( Bool enable )
    {
        m_lightRenderer.SetDebugDraw( enable );
    }

    Bool FrameRenderer::GetLightDebugDraw() const
    {
        return m_lightRenderer.GetDebugDraw();
    }

    void FrameRenderer::RenderDebugLine( const Vector& position0, const Vector& position1, const Vector& color )
    {
        m_debugRenderer.RenderLine( position0, position1, color );
    }

    void FrameRenderer::UpdateFrameUniformBuffer( const FrameContext& context )
    {
        F32 fWidth              = (F32)context.m_width;
        F32 fHeight             = (F32)context.m_height;
        const Camera * camera   = context.m_camera;

        FrameParameters params;
        params.m_viewportSize       = Vector4( fWidth, fHeight, 1.0f/fWidth, 1.0f/fHeight );
        params.m_depthRange         = Vector4( camera->m_near, camera->m_far, camera->m_far-camera->m_near, 1.0f/(camera->m_far-camera->m_near) );
        params.m_viewScale          = camera->GetViewScale();
        params.m_ambientSkyLight    = context.m_scene->GetAmbientSkyLight();
        params.m_ambientGroundLight = context.m_scene->GetAmbientGroundLight();
        params.m_viewMatrix         = camera->GetViewMatrix();
        params.m_projMatrix         = camera->GetProjMatrix();
        params.m_viewProjMatrix     = camera->GetViewProjMatrix();

        void * data = RenderDevice::MapUniformBuffer( m_frameUniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(FrameParameters) );
        RenderDevice::UnmapUniformBuffer();
    }

    void FrameRenderer::LinearizeDepth( Handle depthStencilTexture )
    {
        m_renderCache.SetSRGBWrite( false );
        m_renderCache.SetRenderState( m_renderStateLinearDepth );

        RenderDevice::BindUniformBuffer( m_frameUniformBuffer, LI_FRAME );

        ProgramCache::UseProgram( m_programLinearDepth );

        RenderDevice::BindTexture( depthStencilTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }

    void FrameRenderer::ApplyToneMapping( Handle lightTexture )
    {
        m_renderCache.SetSRGBWrite( true );
        m_renderCache.SetRenderState( m_renderStateToneMapping );

        ProgramCache::UseProgram( m_programToneMapping );

        RenderDevice::BindTexture( lightTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }
}

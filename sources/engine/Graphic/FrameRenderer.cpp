#include "Graphic/FrameRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"

namespace Graphic
{
    FrameRenderer::FrameRenderer()
    {

    }

    FrameRenderer::~FrameRenderer()
    {

    }

    void FrameRenderer::Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer )
    {
        m_debugRenderer                             = debugRenderer;
        m_meshRenderer                              = meshRenderer;
        m_lightRenderer                             = lightRenderer;

        const U32 programLinearDepthId              = ProgramCache::CreateId( "linearDepth" );
        m_programLinearDepth                        = ProgramCache::GetProgram( programLinearDepthId );

        const U32 programToneMappingId              = ProgramCache::CreateId( "toneMapping" );
        m_programToneMapping                        = ProgramCache::GetProgram( programToneMappingId );

        m_renderStateLinearDepth.m_depthWriteMask   = false;
        m_renderStateToneMapping.m_depthWriteMask   = false;
    }

    void FrameRenderer::Destroy()
    {
    }

    FrameRenderer::Context * FrameRenderer::CreateContext()
    {
        Context  * context = MemoryManager::New< Context >();
        
        context->m_width                        = 0;
        context->m_height                       = 0;
        context->m_camera                       = 0;
        context->m_scene                        = 0;
        context->m_depthStencilTexture          = 0;
        context->m_normalTexture                = 0;
        context->m_colorTexture                 = 0;
        context->m_linearDepthTexture           = 0;
        context->m_lightTexture                 = 0;
        context->m_finalColorBuffer             = 0;
        context->m_geomFramebuffer              = RenderDevice::CreateFramebuffer();
        context->m_linearizeDepthFramebuffer    = RenderDevice::CreateFramebuffer();
        context->m_lightFramebuffer             = RenderDevice::CreateFramebuffer();
        context->m_finalFramebuffer             = RenderDevice::CreateFramebuffer();
        context->m_uniformBuffer                = RenderDevice::CreateUniformBuffer( sizeof(FrameParameters), 0, BU_DYNAMIC );
        context->m_debugRendererContext         = DebugRenderer::CreateContext();
        context->m_meshRendererContext          = MeshRenderer::CreateContext( &context->m_opaqueList );
        context->m_lightRendererContext         = LightRenderer::CreateContext( context->m_debugRendererContext );

        RenderState opaque;
        opaque.m_enableDepthTest    = true;
        opaque.m_enableCullFace     = true;

        context->m_opaqueList.SetRenderState( opaque );
        context->m_opaqueList.SetClearMask( RM_ALL );

        return context;
    }

    void FrameRenderer::UpdateContext( Context * context, SizeT width, SizeT height, Camera * camera, Scene * scene )
    {
        context->m_camera    = camera;
        context->m_scene     = scene;

        CARBON_ASSERT( width );
        CARBON_ASSERT( height );

        if ( width != context->m_width || height != context->m_height )
        {
            RenderDevice::DestroyRenderbuffer( context->m_finalColorBuffer );
            RenderDevice::DestroyTexture( context->m_lightTexture );
            RenderDevice::DestroyTexture( context->m_linearDepthTexture );
            RenderDevice::DestroyTexture( context->m_colorTexture );
            RenderDevice::DestroyTexture( context->m_normalTexture );
            RenderDevice::DestroyTexture( context->m_depthStencilTexture );

            context->m_width                = width;
            context->m_height               = height;
            context->m_depthStencilTexture  = RenderDevice::CreateRenderTexture( TF_D24S8, width, height );
            context->m_normalTexture        = RenderDevice::CreateRenderTexture( TF_RG16F, width, height );
            context->m_colorTexture         = RenderDevice::CreateRenderTexture( TF_SRGBA8, width, height );
            context->m_linearDepthTexture   = RenderDevice::CreateRenderTexture( TF_R32F, width, height );
            context->m_lightTexture         = RenderDevice::CreateRenderTexture( TF_RGBA16F, width, height );
            context->m_finalColorBuffer     = RenderDevice::CreateRenderbuffer( TF_SRGBA8, width, height );
        }

        {
            F32 fWidth                  = (F32)width;
            F32 fHeight                 = (F32)height;

            FrameParameters params;
            params.m_viewportSize       = Vector4( fWidth, fHeight, 1.0f/fWidth, 1.0f/fHeight );
            params.m_depthRange         = Vector4( camera->m_near, camera->m_far, camera->m_far-camera->m_near, 1.0f/(camera->m_far-camera->m_near) );
            params.m_viewScale          = camera->GetViewScaleFar();
            params.m_ambientSkyLight    = context->m_scene->GetAmbientSkyLight();
            params.m_ambientGroundLight = context->m_scene->GetAmbientGroundLight();
            params.m_viewMatrix         = camera->GetViewMatrix();
            params.m_projMatrix         = camera->GetProjMatrix();
            params.m_viewProjMatrix     = camera->GetViewProjMatrix();

            void * data = RenderDevice::MapUniformBuffer( context->m_uniformBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, &params, sizeof(FrameParameters) );
            RenderDevice::UnmapUniformBuffer();
        }

        context->m_opaqueList.Clear();

        DebugRenderer::UpdateContext( context->m_debugRendererContext );
        LightRenderer::UpdateContext( context->m_lightRendererContext, camera, context->m_linearDepthTexture, context->m_normalTexture, context->m_colorTexture );
    }

    void FrameRenderer::DestroyContext( Context * context )
    {
        LightRenderer::DestroyContext( context->m_lightRendererContext );
        MeshRenderer::DestroyContext( context->m_meshRendererContext );
        DebugRenderer::DestroyContext( context->m_debugRendererContext );

        RenderDevice::DestroyBuffer( context->m_uniformBuffer );

        RenderDevice::DestroyFramebuffer( context->m_finalFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_lightFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_linearizeDepthFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_geomFramebuffer );

        RenderDevice::DestroyRenderbuffer( context->m_finalColorBuffer );
        RenderDevice::DestroyTexture( context->m_lightTexture );
        RenderDevice::DestroyTexture( context->m_linearDepthTexture );
        RenderDevice::DestroyTexture( context->m_colorTexture );
        RenderDevice::DestroyTexture( context->m_normalTexture );
        RenderDevice::DestroyTexture( context->m_depthStencilTexture );

        MemoryManager::Delete( context );
    }

    void FrameRenderer::Render( Context * context ) const
    {
        const Scene * scene = context->m_scene;

        // Render meshes
        {
            const Scene::MeshArray& meshes = scene->GetMeshes();
            Scene::MeshArray::ConstIterator it  = meshes.Begin();
            Scene::MeshArray::ConstIterator end = meshes.End();
            for ( ; it!=end; ++it )
            {
                m_meshRenderer->Render( *it, context->m_meshRendererContext );
            }
        }

        // Render lights
        {
            const Scene::LightArray& lights = scene->GetLights();
            Scene::LightArray::ConstIterator it  = lights.Begin();
            Scene::LightArray::ConstIterator end = lights.End();
            for ( ; it!=end; ++it )
            {
                m_lightRenderer->Render( *it, context->m_lightRendererContext );
            }
        }
    }

    void FrameRenderer::Draw( Context * context, RenderCache& renderCache ) const
    {
        RenderDevice::BindUniformBuffer( context->m_uniformBuffer, LI_FRAME );

        // Draw G-Buffer
        {
            RenderDevice::BindFramebuffer( context->m_geomFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, context->m_depthStencilTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context->m_normalTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR1, context->m_colorTexture, 0 );
            RenderDevice::DrawBuffer( CBB_COLOR0 | CBB_COLOR1 );

            context->m_opaqueList.Draw( renderCache );
        }

        // Linearize depth
        {
            RenderDevice::BindFramebuffer( context->m_linearizeDepthFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context->m_linearDepthTexture, 0 );

            LinearizeDepth( context, renderCache );
        }

        RenderDevice::BindUniformBuffer( context->m_uniformBuffer, LI_FRAME );

        // Draw Lights
        {
            RenderDevice::BindFramebuffer( context->m_lightFramebuffer, FT_DRAW );
            RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, context->m_depthStencilTexture, 0 );
            RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context->m_lightTexture, 0 );

            m_lightRenderer->Draw( context->m_lightRendererContext, renderCache );
        }

        {
            RenderDevice::BindFramebuffer( context->m_finalFramebuffer, FT_BOTH );
            RenderDevice::AttachTexture( FT_BOTH, FA_DEPTH_STENCIL, context->m_depthStencilTexture, 0 );
            RenderDevice::AttachRenderbuffer( FT_BOTH, FA_COLOR0, context->m_finalColorBuffer );

            // Tone mapping
            ApplyToneMapping( context, renderCache );

            // Debug
            m_debugRenderer->Draw( context->m_debugRendererContext, renderCache );

            // Blit
            RenderDevice::BindFramebuffer( 0, FT_DRAW );
            RenderDevice::DrawBuffer( CBB_BACK );
            RenderDevice::ReadBuffer( CB_COLOR0 );
            RenderDevice::BlitFramebuffer( 0, 0, context->m_width, context->m_height, 0, 0, context->m_width, context->m_height, RM_COLOR, FT_POINT );
        }
    }

    void FrameRenderer::LinearizeDepth( Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( false );
        renderCache.SetRenderState( m_renderStateLinearDepth );

        ProgramCache::UseProgram( m_programLinearDepth );

        RenderDevice::BindTexture( context->m_depthStencilTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }

    void FrameRenderer::ApplyToneMapping( Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( true );
        renderCache.SetRenderState( m_renderStateToneMapping );

        ProgramCache::UseProgram( m_programToneMapping );

        RenderDevice::BindTexture( context->m_lightTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }
}

#include "Graphic/FrameRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"

#include "Core/Math.h"
#include "Core/Trace.h"
#include "Core/StringUtils.h"

namespace Graphic
{
    FrameRenderer::FrameRenderer()
    {

    }

    FrameRenderer::~FrameRenderer()
    {

    }

    void FrameRenderer::Initialize( DebugRenderer * debugRenderer, Rasterizer * rasterizer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer, EnvMapRenderer * envMapRenderer )
    {
        m_debugRenderer                             = debugRenderer;
        m_rasterizer								= rasterizer;
        m_meshRenderer                              = meshRenderer;
        m_lightRenderer                             = lightRenderer;
        m_envMapRenderer                            = envMapRenderer;

        const U32 programLinearDepthId              = ProgramCache::CreateId( "linearDepth" );
        m_programLinearDepth                        = ProgramCache::GetProgram( programLinearDepthId );

        const U32 programToneMappingId              = ProgramCache::CreateId( "toneMapping" );
        m_programToneMapping                        = ProgramCache::GetProgram( programToneMappingId );

        const U32 programOverlayId                  = ProgramCache::CreateId( "overlay" );
        m_programOverlay                            = ProgramCache::GetProgram( programOverlayId );
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
        context->m_rasterizerContext			= Rasterizer::CreateContext();
        context->m_meshRendererContext          = MeshRenderer::CreateContext( &context->m_opaqueList );
        context->m_lightRendererContext         = LightRenderer::CreateContext( context->m_debugRendererContext );
        context->m_envMapRendererContext        = EnvMapRenderer::CreateContext();

        RenderState opaque;
        opaque.m_enableDepthTest    = true;
        opaque.m_enableCullFace     = true;

        context->m_opaqueList.SetRenderState( opaque );
        context->m_opaqueList.SetClearMask( RM_ALL );

        return context;
    }

    void FrameRenderer::UpdateContext( Context * context, SizeT width, SizeT height, Camera * camera, Scene * scene, SizeT envMapSize )
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

            FrameParameters * params	= reinterpret_cast< FrameParameters * >( RenderDevice::MapUniformBuffer( context->m_uniformBuffer, BA_WRITE_ONLY ) );

            params->m_viewportSize      = Vector4( fWidth, fHeight, 1.0f/fWidth, 1.0f/fHeight );
            params->m_depthRange        = Vector4( camera->m_near, camera->m_far, camera->m_far-camera->m_near, 1.0f/(camera->m_far-camera->m_near) );
            params->m_viewScale         = camera->GetViewScaleFar();
            params->m_ambientSkyLight   = context->m_scene->GetAmbientSkyLight();
            params->m_ambientGroundLight= context->m_scene->GetAmbientGroundLight();
            params->m_cameraPosition    = camera->m_position;
            params->m_viewMatrix        = camera->GetViewMatrix();
            params->m_projMatrix        = camera->GetProjMatrix();
            params->m_viewProjMatrix    = camera->GetViewProjMatrix();

            RenderDevice::UnmapUniformBuffer();
        }

        context->m_opaqueList.Clear();

        DebugRenderer::UpdateContext( context->m_debugRendererContext );
        Rasterizer::UpdateContext( context->m_rasterizerContext, width, height, camera, context->m_linearDepthTexture );
        EnvMapRenderer::UpdateContext( context->m_envMapRendererContext, envMapSize, camera, scene );
        LightRenderer::UpdateContext( context->m_lightRendererContext, width, height, camera, context->m_linearDepthTexture, context->m_normalTexture, context->m_colorTexture, context->m_envMapRendererContext->m_lightBlurTexture );
    }

    void FrameRenderer::DestroyContext( Context * context )
    {
		context->m_opaqueList.Clear();

        EnvMapRenderer::DestroyContext( context->m_envMapRendererContext );
        LightRenderer::DestroyContext( context->m_lightRendererContext );
        MeshRenderer::DestroyContext( context->m_meshRendererContext );
        Rasterizer::DestroyContext( context->m_rasterizerContext );
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

        //m_envMapRenderer->Render( context->m_envMapRendererContext );

        const Light ** lights = reinterpret_cast< const Light ** >( MemoryManager::Malloc( sizeof(Light*) * scene->GetLights().Size() ) );
        SizeT lightCount = 0;
        {
            Camera::ApplyFrustumCulling( context->m_camera, scene->GetLights().ConstPtr(), scene->GetLights().Size(), lights, lightCount );

            m_rasterizer->Render( lights, lightCount, context->m_rasterizerContext );
        }

        // Render meshes
        {
            const Scene::MeshArray& meshes = scene->GetMeshes();
            m_meshRenderer->Render( meshes.ConstPtr(), meshes.Size(), context->m_meshRendererContext );
        }

        // Render lights
        {
            m_lightRenderer->Render( lights, lightCount, context->m_lightRendererContext );
        }

        MemoryManager::Free( lights );
    }

    void FrameRenderer::Draw( const Context * context, RenderCache& renderCache ) const
    {
        //m_envMapRenderer->Draw( context->m_envMapRendererContext, renderCache );

        RenderDevice::SetViewport( 0, 0, context->m_width, context->m_height );

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

        m_rasterizer->Draw( context->m_rasterizerContext );

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

            // Overlay
            //DrawOverlay( context, renderCache );

            // Debug
            m_debugRenderer->Draw( context->m_debugRendererContext, renderCache );

            // Blit
            RenderDevice::BindFramebuffer( 0, FT_DRAW );
            RenderDevice::DrawBuffer( CBB_BACK );
            RenderDevice::ReadBuffer( CB_COLOR0 );
            RenderDevice::BlitFramebuffer( 0, 0, context->m_width, context->m_height, 0, 0, context->m_width, context->m_height, RM_COLOR, FT_POINT );
        }
    }

    void FrameRenderer::LinearizeDepth( const Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( false );
        renderCache.SetRenderState( m_renderStateLinearDepth );

        ProgramCache::UseProgram( m_programLinearDepth );

        RenderDevice::BindTexture( context->m_depthStencilTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }

    void FrameRenderer::ApplyToneMapping( const Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( true );
        renderCache.SetRenderState( m_renderStateToneMapping );

        ProgramCache::UseProgram( m_programToneMapping );

        RenderDevice::BindTexture( context->m_lightTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }

    void FrameRenderer::DrawOverlay( const Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( true );
        renderCache.SetRenderState( m_renderStateOverlay );

        ProgramCache::UseProgram( m_programOverlay );

        RenderDevice::BindTexture( context->m_lightRendererContext->m_linearDepthTexture, 0 );
        RenderDevice::BindTexture( context->m_lightRendererContext->m_normalTexture, 1 );
        RenderDevice::BindTexture( context->m_lightRendererContext->m_colorTexture, 2 );
        RenderDevice::BindTextureCube( context->m_envMapRendererContext->m_lightTexture, 3 );
        RenderDevice::BindTextureCube( context->m_envMapRendererContext->m_lightBlurTexture, 4 );
        RenderDevice::BindTexture( context->m_lightTexture, 5 );

        QuadGeometry::GetInstance().Draw();
    }
}

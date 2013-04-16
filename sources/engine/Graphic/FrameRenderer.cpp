#include "Graphic/FrameRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"

#include "Core/Math.h"

namespace Graphic
{
    FrameRenderer::FrameRenderer()
    {

    }

    FrameRenderer::~FrameRenderer()
    {

    }

    void FrameRenderer::Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer, EnvMapRenderer * envMapRenderer )
    {
        m_debugRenderer                             = debugRenderer;
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

            FrameParameters params;
            params.m_viewportSize       = Vector4( fWidth, fHeight, 1.0f/fWidth, 1.0f/fHeight );
            params.m_depthRange         = Vector4( camera->m_near, camera->m_far, camera->m_far-camera->m_near, 1.0f/(camera->m_far-camera->m_near) );
            params.m_viewScale          = camera->GetViewScaleFar();
            params.m_ambientSkyLight    = context->m_scene->GetAmbientSkyLight();
            params.m_ambientGroundLight = context->m_scene->GetAmbientGroundLight();
            params.m_cameraPosition     = camera->m_position;
            params.m_viewMatrix         = camera->GetViewMatrix();
            params.m_projMatrix         = camera->GetProjMatrix();
            params.m_viewProjMatrix     = camera->GetViewProjMatrix();

            void * data = RenderDevice::MapUniformBuffer( context->m_uniformBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, &params, sizeof(FrameParameters) );
            RenderDevice::UnmapUniformBuffer();
        }

        context->m_opaqueList.Clear();

        DebugRenderer::UpdateContext( context->m_debugRendererContext );
        EnvMapRenderer::UpdateContext( context->m_envMapRendererContext, envMapSize, camera, scene );
        LightRenderer::UpdateContext( context->m_lightRendererContext, width, height, camera, context->m_linearDepthTexture, context->m_normalTexture, context->m_colorTexture, context->m_envMapRendererContext->m_lightBlurTexture );
    }

    void FrameRenderer::DestroyContext( Context * context )
    {
        EnvMapRenderer::DestroyContext( context->m_envMapRendererContext );
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

        m_envMapRenderer->Render( context->m_envMapRendererContext );

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

            /*{
                Vector colors[] =
                {
                    Vector4( 0.0f, 0.0f, 0.0f ),
                    Vector4( 1.0f, 0.0f, 0.0f ),
                    Vector4( 0.0f, 1.0f, 0.0f ),
                    Vector4( 1.0f, 1.0f, 0.0f ),
                    Vector4( 0.0f, 0.0f, 1.0f ),
                    Vector4( 1.0f, 0.0f, 1.0f ),
                    Vector4( 0.0f, 1.0f, 1.0f ),
                    Vector4( 1.0f, 1.0f, 1.0f )
                };
                const FrustumQuadTree& quadTree = context->m_lightRendererContext->m_quadTree;
                const FrustumQuadTree::Cell * ptr = quadTree.GetCells();
                for ( SizeT k=0; k<quadTree.GetLevelCount(); ++k )
                {
                    Vector color = colors[k];

                    SizeT size = 1 << k;
                    for ( SizeT i=0; i<size; ++i )
                    {
                        for ( SizeT j=0; j<size; ++j )
                        {
                            const FrustumQuadTree::Cell * cell = ptr++;
                            
                            Vector dirX = ( Cross( UnitY, cell->vPlane ) );
                            Vector dirY = ( Cross( cell->hPlane, UnitX ) );
                            Vector dir = dirX + dirY;

                            const F32 debug_scale = 0.2f;
                            Vector dirA = Splat( debug_scale * k ) * dir;
                            Vector dirB = Splat( debug_scale * ( k + 1 ) ) * dir;

                            DebugLine line = { Select( dirA, UnitW, Mask<0,0,0,1>() ), Select( dirB, UnitW, Mask<0,0,0,1>() ), color };
                            m_debugRenderer->Render( line, context->m_debugRendererContext );
                        }
                    }
                }
            }*/
        }
    }

    void FrameRenderer::Draw( Context * context, RenderCache& renderCache ) const
    {
        m_envMapRenderer->Draw( context->m_envMapRendererContext, renderCache );

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

    void FrameRenderer::DrawOverlay( Context * context, RenderCache& renderCache ) const
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

#include "Graphic/EnvMapRenderer.h"

#include "Graphic/FrameRenderer.h"
#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/QuadGeometry.h"

#include "Core/Quaternion.h"

namespace Graphic
{
    void EnvMapRenderer::Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer )
    {
        m_debugRenderer = debugRenderer;
        m_meshRenderer = meshRenderer;
        m_lightRenderer = lightRenderer;

        const U32 programLinearDepthId  = ProgramCache::CreateId( "linearDepth" );
        m_programLinearDepth            = ProgramCache::GetProgram( programLinearDepthId );

        m_renderStateLinearDepth.m_depthWriteMask = false;
    }

    void EnvMapRenderer::Destroy()
    {
    }

    EnvMapRenderer::Context * EnvMapRenderer::CreateContext()
    {
        Context  * context = MemoryManager::New< Context >();

        context->m_size                         = 0;
        context->m_camera                       = 0;
        context->m_scene                        = 0;
        context->m_depthStencilTexture          = 0;
        context->m_normalTexture                = 0;
        context->m_colorTexture                 = 0;
        context->m_linearDepthTexture           = 0;
        context->m_lightTexture                 = 0;
        context->m_geomFramebuffer              = RenderDevice::CreateFramebuffer();
        context->m_linearizeDepthFramebuffer    = RenderDevice::CreateFramebuffer();
        context->m_lightFramebuffer             = RenderDevice::CreateFramebuffer();

        RenderState opaque;
        opaque.m_enableDepthTest    = true;
        opaque.m_enableCullFace     = true;

        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face                 = context->m_cube[i];
            face.m_uniformBuffer                = RenderDevice::CreateUniformBuffer( sizeof(FrameParameters), 0, BU_DYNAMIC );
            face.m_debugRendererContext         = DebugRenderer::CreateContext();
            face.m_meshRendererContext          = MeshRenderer::CreateContext( &face.m_opaqueList );
            face.m_lightRendererContext         = LightRenderer::CreateContext( face.m_debugRendererContext );

            face.m_opaqueList.SetRenderState( opaque );
            face.m_opaqueList.SetClearMask( RM_ALL );
        }

        return context;
    }

    void EnvMapRenderer::Update( Context * context, SizeT size, Camera * camera, Scene * scene )
    {
        context->m_camera    = camera;
        context->m_scene     = scene;

        CARBON_ASSERT( size );

        if ( size != context->m_size )
        {
            RenderDevice::DestroyTexture( context->m_lightTexture );
            RenderDevice::DestroyTexture( context->m_linearDepthTexture );
            RenderDevice::DestroyTexture( context->m_colorTexture );
            RenderDevice::DestroyTexture( context->m_normalTexture );
            RenderDevice::DestroyTexture( context->m_depthStencilTexture );

            context->m_size                 = size;
            context->m_depthStencilTexture  = RenderDevice::CreateRenderTexture( TF_D24S8, size, size );
            context->m_normalTexture        = RenderDevice::CreateRenderTexture( TF_RG16F, size, size );
            context->m_colorTexture         = RenderDevice::CreateRenderTexture( TF_SRGBA8, size, size );
            context->m_linearDepthTexture   = RenderDevice::CreateRenderTexture( TF_R32F, size, size );
            context->m_lightTexture         = RenderDevice::CreateRenderTexture( TF_RGBA16F, size, size );
        }

        const Vector cube_ori[] =
        {
            Quaternion(UnitX,0.0f),
            Quaternion(UnitY,Pi),
            Quaternion(UnitX,-HalfPi),
            Quaternion(UnitX,HalfPi),
            Quaternion(UnitY,-HalfPi),
            Quaternion(UnitY,HalfPi),
        };

        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face         = context->m_cube[i];

            F32 fSize                   = (F32)size;

            Camera& face_cam            = face.m_camera;
            face_cam.m_position         = camera->m_position;
            face_cam.m_orientation      = cube_ori[i];
            face_cam.m_width            = fSize;
            face_cam.m_height           = fSize;
            face_cam.m_fov              = HalfPi;
            face_cam.m_aspectRatio      = 1.0f;
            face_cam.m_near             = 0.25f;
            face_cam.m_far              = 100.0f;
            face_cam.m_projectionType   = PT_PERSPECTIVE;
            face_cam.Update();

            FrameParameters params;
            params.m_viewportSize       = Vector4( fSize, fSize, 1.0f/fSize, 1.0f/fSize );
            params.m_depthRange         = Vector4( face_cam.m_near, face_cam.m_far, face_cam.m_far-face_cam.m_near, 1.0f/(face_cam.m_far-face_cam.m_near) );
            params.m_viewScale          = face_cam.GetViewScaleFar();
            params.m_ambientSkyLight    = context->m_scene->GetAmbientSkyLight();
            params.m_ambientGroundLight = context->m_scene->GetAmbientGroundLight();
            params.m_viewMatrix         = face_cam.GetViewMatrix();
            params.m_projMatrix         = face_cam.GetProjMatrix();
            params.m_viewProjMatrix     = face_cam.GetViewProjMatrix();

            void * data = RenderDevice::MapUniformBuffer( face.m_uniformBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, &params, sizeof(FrameParameters) );
            RenderDevice::UnmapUniformBuffer();

            face.m_opaqueList.Clear();

            DebugRenderer::UpdateContext( face.m_debugRendererContext );
            LightRenderer::UpdateContext( face.m_lightRendererContext, &face_cam, context->m_depthStencilTexture, context->m_normalTexture, context->m_colorTexture );
        }        
    }

    void EnvMapRenderer::DestroyContext( Context * context )
    {
        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face = context->m_cube[i];

            LightRenderer::DestroyContext( face.m_lightRendererContext );
            MeshRenderer::DestroyContext( face.m_meshRendererContext );
            DebugRenderer::DestroyContext( face.m_debugRendererContext );

            RenderDevice::DestroyBuffer( face.m_uniformBuffer );
        }

        RenderDevice::DestroyFramebuffer( context->m_lightFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_linearizeDepthFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_geomFramebuffer );

        RenderDevice::DestroyTexture( context->m_lightTexture );
        RenderDevice::DestroyTexture( context->m_linearDepthTexture );
        RenderDevice::DestroyTexture( context->m_colorTexture );
        RenderDevice::DestroyTexture( context->m_normalTexture );
        RenderDevice::DestroyTexture( context->m_depthStencilTexture );

        MemoryManager::Delete( context );
    }

    void EnvMapRenderer::Render( Context * context, RenderCache& renderCache ) const
    {
        const Scene * scene = context->m_scene;

        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face = context->m_cube[i];

            RenderDevice::BindUniformBuffer( face.m_uniformBuffer, LI_FRAME );

            // Render meshes
            {
                const Scene::MeshArray& meshes = scene->GetMeshes();
                Scene::MeshArray::ConstIterator it  = meshes.Begin();
                Scene::MeshArray::ConstIterator end = meshes.End();
                for ( ; it!=end; ++it )
                {
                    m_meshRenderer->Render( *it, face.m_meshRendererContext );
                }
            }

            // Render lights
            {
                const Scene::LightArray& lights = scene->GetLights();
                Scene::LightArray::ConstIterator it  = lights.Begin();
                Scene::LightArray::ConstIterator end = lights.End();
                for ( ; it!=end; ++it )
                {
                    m_lightRenderer->Render( *it, face.m_lightRendererContext );
                }
            }
        }
    }

    void EnvMapRenderer::Draw( Context * context, RenderCache& renderCache ) const
    {
        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face = context->m_cube[i];

            // Draw G-Buffer
            {
                RenderDevice::BindFramebuffer( context->m_geomFramebuffer, FT_DRAW );
                RenderDevice::AttachTexture( FT_DRAW, FA_DEPTH_STENCIL, context->m_depthStencilTexture, 0 );
                RenderDevice::AttachTexture( FT_DRAW, FA_COLOR0, context->m_normalTexture, 0 );
                RenderDevice::AttachTexture( FT_DRAW, FA_COLOR1, context->m_colorTexture, 0 );
                RenderDevice::DrawBuffer( CBB_COLOR0 | CBB_COLOR1 );

                face.m_opaqueList.Draw( renderCache );
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

                m_lightRenderer->Draw( face.m_lightRendererContext, renderCache );
            }
        }
    }

    void EnvMapRenderer::LinearizeDepth( Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( false );
        renderCache.SetRenderState( m_renderStateLinearDepth );

        ProgramCache::UseProgram( m_programLinearDepth );

        RenderDevice::BindTexture( context->m_depthStencilTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }
}

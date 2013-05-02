#include "Graphic/EnvMapRenderer.h"

#include "Graphic/FrameRenderer.h"
#include "Graphic/RenderDevice.h"
#include "Graphic/Scene.h"
#include "Graphic/QuadGeometry.h"

#include "Core/Quaternion.h"

namespace Graphic
{
    const SizeT envBlurRadius   = 32;
    const SizeT envBlurSize     = 2 * envBlurRadius + 1;
    const F32 envBlurSigma      = 12.0f;

    void EnvMapRenderer::Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer )
    {
        m_debugRenderer = debugRenderer;
        m_meshRenderer = meshRenderer;
        m_lightRenderer = lightRenderer;

        const U32 programLinearDepthId              = ProgramCache::CreateId( "linearDepth" );
        m_programLinearDepth                        = ProgramCache::GetProgram( programLinearDepthId );

        const U32 programBlurYId                    = ProgramCache::CreateId( "blurRingY" );
        m_programBlur[0]                            = ProgramCache::GetProgram( programBlurYId );

        const U32 programBlurZId                    = ProgramCache::CreateId( "blurRingZ" );
        m_programBlur[1]                            = ProgramCache::GetProgram( programBlurZId );

        const U32 programBlurXId                    = ProgramCache::CreateId( "blurRingX" );
        m_programBlur[2]                            = ProgramCache::GetProgram( programBlurXId );

        m_renderStateLinearDepth.m_depthWriteMask   = false;

        F32 sum = 0.0f;
        F32 blurWeights[ envBlurSize ];
        for ( SizeT i=0; i<envBlurSize; ++i )
        {
            F32 x = (F32)i;
            x -= (F32)envBlurRadius;
            F32 g = Exp( -(x*x)/(2.0f*envBlurSigma*envBlurSigma) );

            blurWeights[i] = g;
            sum += g;
        }
        for ( SizeT i=0; i<envBlurSize; ++i )
        {
            blurWeights[i] /= sum;
        }

        m_blurUniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(blurWeights), blurWeights, BU_STATIC );
    }

    void EnvMapRenderer::Destroy()
    {
        RenderDevice::DestroyBuffer( m_blurUniformBuffer );
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
        context->m_lightBlurTexture             = 0;
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

    void EnvMapRenderer::UpdateContext( Context * context, SizeT size, Camera * camera, Scene * scene )
    {
        context->m_camera    = camera;
        context->m_scene     = scene;

        CARBON_ASSERT( size );

        if ( size != context->m_size )
        {
            RenderDevice::DestroyTexture( context->m_lightBlurTexture );
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
            context->m_lightTexture         = RenderDevice::CreateRenderTextureCube( TF_RGBA16F, size );
            context->m_lightBlurTexture     = RenderDevice::CreateRenderTextureCube( TF_RGBA16F, size );
        }

        const Vector cube_ori_align[] =
        {
            MulQuat( Quaternion(UnitX,Pi), Quaternion(UnitY,-HalfPi) ),
            MulQuat( Quaternion(UnitX,Pi), Quaternion(UnitY,HalfPi) ),
            Quaternion(UnitX,HalfPi),
            Quaternion(UnitX,-HalfPi),
            MulQuat( Quaternion(UnitZ,Pi), Quaternion(UnitY,Pi) ),
            MulQuat( Quaternion(UnitZ,Pi), Quaternion(UnitY,0.0f) ),
        };

        const Vector cube_ori[] =
        {
            MulQuat( camera->m_orientation, cube_ori_align[0] ),
            MulQuat( camera->m_orientation, cube_ori_align[1] ),
            MulQuat( camera->m_orientation, cube_ori_align[2] ),
            MulQuat( camera->m_orientation, cube_ori_align[3] ),
            MulQuat( camera->m_orientation, cube_ori_align[4] ),
            MulQuat( camera->m_orientation, cube_ori_align[5] ),
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

			FrameParameters * params	= reinterpret_cast< FrameParameters * >( RenderDevice::MapUniformBuffer( face.m_uniformBuffer, BA_WRITE_ONLY ) );

            params->m_viewportSize      = Vector4( fSize, fSize, 1.0f/fSize, 1.0f/fSize );
            params->m_depthRange        = Vector4( face_cam.m_near, face_cam.m_far, face_cam.m_far-face_cam.m_near, 1.0f/(face_cam.m_far-face_cam.m_near) );
            params->m_viewScale         = face_cam.GetViewScaleFar();
            params->m_ambientSkyLight   = context->m_scene->GetAmbientSkyLight();
            params->m_ambientGroundLight= context->m_scene->GetAmbientGroundLight();
            params->m_viewMatrix        = face_cam.GetViewMatrix();
            params->m_projMatrix        = face_cam.GetProjMatrix();
            params->m_viewProjMatrix    = face_cam.GetViewProjMatrix();

            RenderDevice::UnmapUniformBuffer();

            face.m_opaqueList.Clear();

            DebugRenderer::UpdateContext( face.m_debugRendererContext );
            LightRenderer::UpdateContext( face.m_lightRendererContext, size, size, &face_cam, context->m_linearDepthTexture, context->m_normalTexture, context->m_colorTexture, 0 );
        }        
    }

    void EnvMapRenderer::DestroyContext( Context * context )
    {
        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face = context->m_cube[i];

			face.m_opaqueList.Clear();

            LightRenderer::DestroyContext( face.m_lightRendererContext );
            MeshRenderer::DestroyContext( face.m_meshRendererContext );
            DebugRenderer::DestroyContext( face.m_debugRendererContext );

            RenderDevice::DestroyBuffer( face.m_uniformBuffer );
        }

        RenderDevice::DestroyFramebuffer( context->m_lightFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_linearizeDepthFramebuffer );
        RenderDevice::DestroyFramebuffer( context->m_geomFramebuffer );

        RenderDevice::DestroyTexture( context->m_lightBlurTexture );
        RenderDevice::DestroyTexture( context->m_lightTexture );
        RenderDevice::DestroyTexture( context->m_linearDepthTexture );
        RenderDevice::DestroyTexture( context->m_colorTexture );
        RenderDevice::DestroyTexture( context->m_normalTexture );
        RenderDevice::DestroyTexture( context->m_depthStencilTexture );

        MemoryManager::Delete( context );
    }

    void EnvMapRenderer::Render( Context * context ) const
    {
        const Scene * scene = context->m_scene;

        SizeT inCount = scene->GetLights().Size();
        Vector * sphereBuffer = reinterpret_cast< Vector * >( MemoryManager::Malloc( sizeof(Vector) * inCount, MemoryUtils::AlignOf< Vector >() ) );
        const Vector ** inData = reinterpret_cast< const Vector ** >( MemoryManager::Malloc( sizeof(Vector**) * inCount ) );
        const Vector ** outData = reinterpret_cast< const Vector ** >( MemoryManager::Malloc( sizeof(Vector**) * inCount ) );

        for ( SizeT i=0; i<inCount; ++i )
        {
            const Light * light = scene->GetLights()[i];

            inData[i] = sphereBuffer + i;
            Vector& sphere = sphereBuffer[i];

            sphere = Select( light->m_position, Splat(light->m_radius), Mask<0,0,0,1>() );
        }

        const Light ** lights = reinterpret_cast< const Light ** >( MemoryManager::Malloc( sizeof(const Light **) * inCount ) );

        for ( SizeT i=0; i<6; ++i )
        {
            Context::Face& face = context->m_cube[i];

            SizeT lightCount = 0;
            {
                SizeT outCount;
                Camera::ApplyFrustumCulling( &face.m_camera, inData, inCount, outData, outCount );

                for ( ; lightCount<outCount; ++lightCount )
                {
                    lights[ lightCount ] = scene->GetLights()[ outData[ lightCount ] - sphereBuffer ];
                }
            }

            // Render meshes
            {
                const Scene::MeshArray& meshes = scene->GetMeshes();
                m_meshRenderer->Render( meshes.ConstPtr(), meshes.Size(), face.m_meshRendererContext );
            }

            // Render lights
            {
                m_lightRenderer->Render( lights, lightCount, face.m_lightRendererContext );
            }
        }

        MemoryManager::Free( lights );
        MemoryManager::Free( outData );
        MemoryManager::Free( inData );
        MemoryManager::Free( sphereBuffer );
    }

    void EnvMapRenderer::Draw( const Context * context, RenderCache& renderCache ) const
    {
        RenderDevice::SetViewport( 0, 0, context->m_size, context->m_size );

        for ( SizeT i=0; i<6; ++i )
        {
            const Context::Face& face = context->m_cube[i];

            RenderDevice::BindUniformBuffer( face.m_uniformBuffer, LI_FRAME );

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
                RenderDevice::AttachTextureCube( FT_DRAW, FA_COLOR0, (CubeFace)i, context->m_lightTexture, 0 );

                m_lightRenderer->DrawEnv( face.m_lightRendererContext, renderCache );
            }
        }

        // Blur
        {
            RenderDevice::CopyImageCube( context->m_lightTexture, 0, 0, 0, 0, context->m_lightBlurTexture, 0, 0, 0, 0, context->m_size, context->m_size, 6 );

            RenderDevice::BindUniformBuffer( m_blurUniformBuffer, 0 );

            for ( SizeT i=0; i<3; ++i )
            {
                ProgramCache::UseProgram( m_programBlur[i] );
                RenderDevice::BindImageTextureCube( context->m_lightBlurTexture, 0, 0, BA_READ_WRITE, TF_RGBA16F );
                RenderDevice::DispatchCompute( 1, context->m_size, 1 );
            }
        }
    }

    void EnvMapRenderer::LinearizeDepth( const Context * context, RenderCache& renderCache ) const
    {
        renderCache.SetSRGBWrite( false );
        renderCache.SetRenderState( m_renderStateLinearDepth );

        ProgramCache::UseProgram( m_programLinearDepth );

        RenderDevice::BindTexture( context->m_depthStencilTexture, 0 );

        QuadGeometry::GetInstance().Draw();
    }
}

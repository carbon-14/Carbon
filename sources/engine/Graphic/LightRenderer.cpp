#include "Graphic/LightRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderCache.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"

#include "Core/Math.h"
#include "Core/Matrix.h"

namespace Graphic
{
    struct LightParameters
    {
        Matrix  m_lightMatrix;
        Vector  m_valueInvSqrRadius;
        Vector  m_position;
        Vector  m_direction;
    };

    void LightRenderer::Initialize()
    {
        m_directionalGeometry.Initialize();
        m_omniGeometry.Initialize();
        m_spotGeometry.Initialize();

        // Directional config
        {
            RenderConfig& config    = m_configs[LT_DIRECTIONAL];
            U32 programId           = ProgramCache::CreateId( "lightingDirectional" );
            config.m_program        = ProgramCache::GetProgram( programId );
            config.m_geometry       = &m_directionalGeometry;
            config.m_func           = &Graphic::LightRenderer::DirectionalTransform;
        }
        // Omni config
        {
            RenderConfig& config    = m_configs[LT_OMNI];
            U32 programId           = ProgramCache::CreateId( "lightingOmni" );
            config.m_program        = ProgramCache::GetProgram( programId );
            config.m_geometry       = &m_omniGeometry;
            config.m_func           = &Graphic::LightRenderer::OmniTransform;
        }
        // Spot config
        {
            RenderConfig& config    = m_configs[LT_SPOT];
            U32 programId           = ProgramCache::CreateId( "lightingSpot" );
            config.m_program        = ProgramCache::GetProgram( programId );
            config.m_geometry       = &m_spotGeometry;
            config.m_func           = &Graphic::LightRenderer::SpotTransform;
        }

        U32 maskId                      = ProgramCache::CreateId( "lightingMask" );
        m_mask                          = ProgramCache::GetProgram( maskId );

        m_maskClear0.m_colorWriteMask    = 0;
        m_maskClear0.m_enableCullFace    = true;
        m_maskClear0.m_cullFace          = CF_FRONT;
        m_maskClear0.m_depthWriteMask    = false;
        m_maskClear0.m_enableStencilTest = true;
        m_maskClear0.m_stencilRef        = 1;
        m_maskClear0.m_stencilFunc       = F_ALWAYS;
        m_maskClear0.m_stencilFail       = O_KEEP;
        m_maskClear0.m_depthFail         = O_KEEP;
        m_maskClear0.m_depthPass         = O_ZERO;

        m_maskClear1.m_colorWriteMask    = 0;
        m_maskClear1.m_enableCullFace    = true;
        m_maskClear1.m_cullFace          = CF_FRONT;
        m_maskClear1.m_enableDepthTest   = false;
        m_maskClear1.m_depthWriteMask    = false;
        m_maskClear1.m_enableStencilTest = true;
        m_maskClear1.m_stencilRef        = 1;
        m_maskClear1.m_stencilFunc       = F_ALWAYS;
        m_maskClear1.m_stencilFail       = O_KEEP;
        m_maskClear1.m_depthFail         = O_KEEP;
        m_maskClear1.m_depthPass         = O_REPLACE;

        m_maskFront.m_colorWriteMask    = 0;
        m_maskFront.m_enableCullFace    = true;
        m_maskFront.m_cullFace          = CF_BACK;
        m_maskFront.m_enableDepthTest   = true;
        m_maskFront.m_depthWriteMask    = false;
        m_maskFront.m_enableStencilTest = true;
        m_maskFront.m_stencilRef        = 1;
        m_maskFront.m_stencilFunc       = F_ALWAYS;
        m_maskFront.m_stencilFail       = O_KEEP;
        m_maskFront.m_depthFail         = O_KEEP;
        m_maskFront.m_depthPass         = O_REPLACE;

        m_maskBack.m_colorWriteMask     = 0;
        m_maskBack.m_enableCullFace     = true;
        m_maskBack.m_cullFace           = CF_FRONT;
        m_maskBack.m_enableDepthTest    = true;
        m_maskBack.m_depthWriteMask     = false;
        m_maskBack.m_enableStencilTest  = true;
        m_maskBack.m_stencilRef         = 1;
        m_maskBack.m_stencilFunc        = F_ALWAYS;
        m_maskBack.m_stencilFail        = O_KEEP;
        m_maskBack.m_depthFail          = O_KEEP;
        m_maskBack.m_depthPass          = O_ZERO;

        m_lighting.m_enableCullFace     = true;
        m_lighting.m_cullFace           = CF_FRONT;
        m_lighting.m_enableDepthTest    = false;
        m_lighting.m_depthWriteMask     = false;
        m_lighting.m_enableStencilTest  = true;
        m_lighting.m_stencilRef         = 1;
        m_lighting.m_stencilFunc        = F_EQUAL;
        m_lighting.m_stencilFail        = O_KEEP;
        m_lighting.m_depthFail          = O_KEEP;
        m_lighting.m_depthPass          = O_KEEP;
        m_lighting.m_enableBlend        = true;
        m_lighting.m_srcBlendFunc       = BF_ONE;
        m_lighting.m_dstBlendFunc       = BF_ONE;
        m_lighting.m_blendMode          = BM_ADD;

        m_uniformBufferCount = 0;
    }

    void LightRenderer::Destroy()
    {
        UniformBufferArray::Iterator it = m_uniformBufferPool.Begin();
        UniformBufferArray::Iterator end = m_uniformBufferPool.End();
        for ( ; it!=end; ++it )
        {
            RenderDevice::DestroyBuffer( *it );
        }

        m_spotGeometry.Destroy();
        m_omniGeometry.Destroy();
        m_directionalGeometry.Destroy();
    }

    void LightRenderer::Render( const Scene * scene, const Camera * camera )
    {
        const Scene::LightArray& lights = scene->GetLights();
        Scene::LightArray::ConstIterator it = lights.Begin();
        Scene::LightArray::ConstIterator end = lights.End();
        for ( ; it != end; ++it )
        {
            const Light * light = *it;

            Bool useFrontFaceMask;
            Vector lightDirection;
            const Matrix transform  = ComputeLightTransform( light, camera, lightDirection, useFrontFaceMask );

            F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

            LightParameters params;
            params.m_lightMatrix        = Mul( camera->GetViewProjMatrix(), transform );
            params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
            params.m_position           = TransformVertex( camera->GetViewMatrix(), light->m_position );
            params.m_direction          = TransformVector( camera->GetViewMatrix(), lightDirection );

            Handle uniformBuffer = GetUniformBuffer();
            void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
            MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
            RenderDevice::UnmapUniformBuffer();

            RenderLight renderLight = { light->m_type, uniformBuffer, useFrontFaceMask };
            m_lights.PushBack( renderLight );
        }
    }

    void LightRenderer::Draw( RenderCache& renderCache, Handle frameParameters, Handle depthStencilTexture, Handle normalTexture, Handle colorTexture )
    {
        RenderDevice::BindTexture( depthStencilTexture  , 0 );
        RenderDevice::BindTexture( normalTexture        , 1 );
        RenderDevice::BindTexture( colorTexture         , 2 );

        renderCache.SetClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        RenderDevice::Clear( RM_COLOR );

        renderCache.SetSRGBWrite( false );

        LightArray::ConstIterator it = m_lights.Begin();
        LightArray::ConstIterator end = m_lights.End();
        for ( ; it!=end; ++it )
        {
            const RenderConfig& config = m_configs[ it->m_type ];

            RenderDevice::BindUniformBuffer( frameParameters, LI_FRAME );
            RenderDevice::BindUniformBuffer( it->m_uniformBuffer, 0 );

            // Stencil masking
            ProgramCache::UseProgram( m_mask );

            if ( it->m_useFrontFace )
            {
                renderCache.SetRenderState( m_maskClear0 );
                config.m_geometry->Draw();
        
                renderCache.SetRenderState( m_maskFront );
                config.m_geometry->Draw();
            }
            else
            {
                renderCache.SetRenderState( m_maskClear1 );
                config.m_geometry->Draw();
            }

            renderCache.SetRenderState( m_maskBack );
            config.m_geometry->Draw();

            // Lighting 
            ProgramCache::UseProgram( config.m_program );

            renderCache.SetRenderState( m_lighting );
            config.m_geometry->Draw();
        }

        m_lights.Clear();
        m_uniformBufferCount = 0;
    }

    Matrix LightRenderer::ComputeLightTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const
    {
        return (this->*(m_configs[light->m_type].m_func))( light, camera, lightDirection, useFrontFaceMask );
    }

    Matrix LightRenderer::DirectionalTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const
    {
        Matrix ori = RMatrix( light->m_orientation );

        lightDirection = -ori.m_column[2];

        Matrix m = ori;
        Scale( m, Vector4( 0.5f*light->m_directionalWidth, 0.5f*light->m_directionalHeight, light->m_radius ) );
        m.m_column[3] = light->m_position;

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector points[5];
        points[0] = camera->m_position;
        points[1] = camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR];
        points[2] = camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR];
        points[3] = camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR];
        points[4] = camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR];

        Vector normals[6];
        normals[0] = -ori.m_column[0];
        normals[1] = ori.m_column[0];
        normals[2] = -ori.m_column[1];
        normals[3] = ori.m_column[1];
        normals[4] = -ori.m_column[2];
        normals[5] = ori.m_column[2];

        F128 p;
        Store( p, light->m_position );

        Vector planes[6];
        planes[0] = Select( normals[0], -(Splat( 0.5f*light->m_directionalWidth )+Dot( normals[0], light->m_position )), Mask<0,0,0,1>() );
        planes[1] = Select( normals[1], -(Splat( 0.5f*light->m_directionalWidth )+Dot( normals[1], light->m_position )), Mask<0,0,0,1>() );
        planes[2] = Select( normals[2], -(Splat( 0.5f*light->m_directionalHeight )+Dot( normals[2], light->m_position )), Mask<0,0,0,1>() );
        planes[3] = Select( normals[3], -(Splat( 0.5f*light->m_directionalHeight )+Dot( normals[3], light->m_position )), Mask<0,0,0,1>() );
        planes[4] = Select( normals[4], -(Splat( light->m_radius )+Dot( normals[4], light->m_position )), Mask<0,0,0,1>() );
        planes[5] = Select( normals[5], -(Dot( normals[5], light->m_position )), Mask<0,0,0,1>() );

        useFrontFaceMask = true;
        for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
        {
            SizeT j=0;
            for ( ; j<6; ++j )
            {
                F128 d;
                Store( d, Dot( points[i], planes[j] ) );

                if ( d[0] > 0.0f )
                    break;
            }

            useFrontFaceMask = j != 6;
        }

        return m;
    }

    Matrix LightRenderer::OmniTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const
    {
        Matrix m = SMatrix( Splat( light->m_radius ) );
        m.m_column[3] = light->m_position;

        lightDirection = Zero4();

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector points[5];
        points[0] = camera->m_position;
        points[1] = camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR];
        points[2] = camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR];
        points[3] = camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR];
        points[4] = camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR];

        useFrontFaceMask = true;
        for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
        {
            Vector dist = Abs( points[i] - light->m_position );
            dist = Max( dist, Swizzle<1,2,0,3>(dist) );
            dist = Max( dist, Swizzle<2,0,1,3>(dist) );

            F128 d;
            Store( d, dist );

            useFrontFaceMask = d[0] > light->m_radius;
        }

        return m;
    }

    Matrix LightRenderer::SpotTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const
    {
        F32 tangent = light->m_radius * Tan( 0.5f * light->m_spotOutAngle );

        Matrix m = RMatrix( light->m_orientation );
        lightDirection = -m.m_column[2];
        Scale( m, Vector4( tangent, tangent, light->m_radius ) );
        m.m_column[3] = light->m_position;

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector points[5];
        points[0] = camera->m_position;
        points[1] = camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR];
        points[2] = camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR];
        points[3] = camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR];
        points[4] = camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR];

        Vector spot_points[5];
        spot_points[0] = light->m_position;
        spot_points[1] = TransformVertex( m, Vector4( -1.0f, -1.0f, -1.0f ) );
        spot_points[2] = TransformVertex( m, Vector4( +1.0f, -1.0f, -1.0f ) );
        spot_points[3] = TransformVertex( m, Vector4( -1.0f, +1.0f, -1.0f ) );
        spot_points[4] = TransformVertex( m, Vector4( +1.0f, +1.0f, -1.0f ) );

        Vector spot_vecs[4];
        spot_vecs[0] = spot_points[1] - spot_points[0];
        spot_vecs[1] = spot_points[2] - spot_points[0];
        spot_vecs[2] = spot_points[3] - spot_points[0];
        spot_vecs[3] = spot_points[4] - spot_points[0];

        Vector spot_normals[5];
        spot_normals[0] = lightDirection;
        spot_normals[1] = Normalize( Cross( spot_vecs[0], spot_vecs[1] ) );
        spot_normals[2] = Normalize( Cross( spot_vecs[1], spot_vecs[3] ) );
        spot_normals[3] = Normalize( Cross( spot_vecs[3], spot_vecs[2] ) );
        spot_normals[4] = Normalize( Cross( spot_vecs[2], spot_vecs[0] ) );

        Vector planes[5];
        planes[0] = Select( spot_normals[0], -(Splat( light->m_radius )+Dot( spot_normals[0], light->m_position )), Mask<0,0,0,1>() );
        planes[1] = Select( spot_normals[1], -Dot( spot_normals[1], light->m_position ), Mask<0,0,0,1>() );
        planes[2] = Select( spot_normals[2], -Dot( spot_normals[2], light->m_position ), Mask<0,0,0,1>() );
        planes[3] = Select( spot_normals[3], -Dot( spot_normals[3], light->m_position ), Mask<0,0,0,1>() );
        planes[4] = Select( spot_normals[4], -Dot( spot_normals[4], light->m_position ), Mask<0,0,0,1>() );

        useFrontFaceMask = true;
        for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
        {
            SizeT j=0;
            for ( ; j<5; ++j )
            {
                F128 d;
                Store( d, Dot( points[i], planes[j] ) );

                if ( d[0] > 0.0f )
                    break;
            }

            useFrontFaceMask = j != 5;
        }

        return m;
    }

    Handle LightRenderer::GetUniformBuffer()
    {
        if ( m_uniformBufferCount == m_uniformBufferPool.Size() )
        {
            m_uniformBufferPool.PushBack( RenderDevice::CreateUniformBuffer( sizeof(LightParameters), 0, BU_DYNAMIC ) );
        }

        return m_uniformBufferPool[ m_uniformBufferCount++ ];
    }
}

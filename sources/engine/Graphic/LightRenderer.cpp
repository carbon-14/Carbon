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
        Vector  m_spotParameters;
    };

    const F32 LightRenderer::ms_distanceThreshold = 0.1f;

    void LightRenderer::Initialize()
    {
        // Directional light
        {
            m_geomDirectional.Initialize();

            U32 programId                   = ProgramCache::CreateId( "lightingDirectional" );
            m_programDirectional            = ProgramCache::GetProgram( programId );
            m_renderFunc[LT_DIRECTIONAL]    = &Graphic::LightRenderer::RenderDirectional;
        }
        // Omni light
        {
            m_geomOmni.Initialize();

            U32 programId                   = ProgramCache::CreateId( "lightingOmni" );
            m_programOmni                   = ProgramCache::GetProgram( programId );
            m_renderFunc[LT_OMNI]           = &Graphic::LightRenderer::RenderOmni;
        }
        // Spot light
        {
            m_geomSpot.Initialize();

            U32 programId                   = ProgramCache::CreateId( "lightingSpot" );
            m_programSpot                   = ProgramCache::GetProgram( programId );
            m_renderFunc[LT_SPOT]           = &Graphic::LightRenderer::RenderSpot;
        }

        U32 maskId                  = ProgramCache::CreateId( "lightingMask" );
        m_programMask               = ProgramCache::GetProgram( maskId );

        U32 ambientMaskId           = ProgramCache::CreateId( "lightingAmbientMask" );
        m_programAmbientMask        = ProgramCache::GetProgram( ambientMaskId );

        U32 ambientLightingId       = ProgramCache::CreateId( "lightingAmbient" );
        m_programAmbient            = ProgramCache::GetProgram( ambientLightingId );

        m_stateMaskClear0.m_colorWriteMask          = 0;
        m_stateMaskClear0.m_enableCullFace          = true;
        m_stateMaskClear0.m_cullFace                = CF_FRONT;
        m_stateMaskClear0.m_depthWriteMask          = false;
        m_stateMaskClear0.m_enableStencilTest       = true;
        m_stateMaskClear0.m_stencilRef              = 1;
        m_stateMaskClear0.m_depthPass               = O_ZERO;

        m_stateMaskClear1.m_colorWriteMask          = 0;
        m_stateMaskClear1.m_enableCullFace          = true;
        m_stateMaskClear1.m_cullFace                = CF_FRONT;
        m_stateMaskClear1.m_depthWriteMask          = false;
        m_stateMaskClear1.m_enableStencilTest       = true;
        m_stateMaskClear1.m_stencilRef              = 1;
        m_stateMaskClear1.m_depthPass               = O_REPLACE;

        m_stateMaskFront.m_colorWriteMask           = 0;
        m_stateMaskFront.m_enableCullFace           = true;
        m_stateMaskFront.m_cullFace                 = CF_BACK;
        m_stateMaskFront.m_enableDepthTest          = true;
        m_stateMaskFront.m_depthWriteMask           = false;
        m_stateMaskFront.m_enableStencilTest        = true;
        m_stateMaskFront.m_stencilRef               = 1;
        m_stateMaskFront.m_depthPass                = O_REPLACE;

        m_stateMaskBack.m_colorWriteMask            = 0;
        m_stateMaskBack.m_enableCullFace            = true;
        m_stateMaskBack.m_cullFace                  = CF_FRONT;
        m_stateMaskBack.m_enableDepthTest           = true;
        m_stateMaskBack.m_depthWriteMask            = false;
        m_stateMaskBack.m_enableStencilTest         = true;
        m_stateMaskBack.m_stencilRef                = 1;
        m_stateMaskBack.m_depthPass                 = O_ZERO;

        m_stateLighting.m_enableCullFace            = true;
        m_stateLighting.m_cullFace                  = CF_FRONT;
        m_stateLighting.m_depthWriteMask            = false;
        m_stateLighting.m_enableStencilTest         = true;
        m_stateLighting.m_stencilRef                = 1;
        m_stateLighting.m_stencilFunc               = F_EQUAL;
        m_stateLighting.m_enableBlend               = true;
        m_stateLighting.m_srcBlendFunc              = BF_ONE;
        m_stateLighting.m_dstBlendFunc              = BF_ONE;
        m_stateLighting.m_blendMode                 = BM_ADD;

        m_stateAmbientMask.m_colorWriteMask         = 0;
        m_stateAmbientMask.m_enableDepthTest        = true;
        m_stateAmbientMask.m_depthWriteMask         = false;
        m_stateAmbientMask.m_enableStencilTest      = true;
        m_stateAmbientMask.m_depthFunc              = F_EQUAL;
        m_stateAmbientMask.m_stencilRef             = 1;
        m_stateAmbientMask.m_depthPass              = O_REPLACE;

        m_stateAmbientLighting.m_depthWriteMask     = false;
        m_stateAmbientLighting.m_enableStencilTest  = true;
        m_stateAmbientLighting.m_stencilRef         = 0;
        m_stateAmbientLighting.m_stencilFunc        = F_EQUAL;

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

        m_geomSpot.Destroy();
        m_geomOmni.Destroy();
        m_geomDirectional.Destroy();
    }

    void LightRenderer::Render( const Scene * scene, const Camera * camera )
    {
        const Scene::LightArray& lights = scene->GetLights();
        Scene::LightArray::ConstIterator it = lights.Begin();
        Scene::LightArray::ConstIterator end = lights.End();
        for ( ; it != end; ++it )
        {
            RenderLightVolume( *it, camera );
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

        RenderDevice::BindUniformBuffer( frameParameters, LI_FRAME );

        ProgramCache::UseProgram( m_programAmbientMask );
        renderCache.SetRenderState( m_stateAmbientMask );
        QuadGeometry::GetInstance().Draw();

        ProgramCache::UseProgram( m_programAmbient );
        renderCache.SetRenderState( m_stateAmbientLighting );
        QuadGeometry::GetInstance().Draw();

        LightArray::ConstIterator it = m_lights.Begin();
        LightArray::ConstIterator end = m_lights.End();
        for ( ; it!=end; ++it )
        {
            const RenderLight& light = *it;

            RenderDevice::BindUniformBuffer( frameParameters, LI_FRAME );
            RenderDevice::BindUniformBuffer( it->m_uniformBuffer, 0 );

            // Stencil masking
            ProgramCache::UseProgram( m_programMask );

            if ( light.m_useFrontFace )
            {
                renderCache.SetRenderState( m_stateMaskClear0 );
                 light.m_geometry->Draw();
        
                renderCache.SetRenderState( m_stateMaskFront );
                 light.m_geometry->Draw();
            }
            else
            {
                renderCache.SetRenderState( m_stateMaskClear1 );
                light.m_geometry->Draw();
            }

            renderCache.SetRenderState( m_stateMaskBack );
            light.m_geometry->Draw();

            // Lighting 
            ProgramCache::UseProgram( light.m_program );

            renderCache.SetRenderState( m_stateLighting );
            light.m_geometry->Draw();
        }

        m_lights.Clear();
        m_uniformBufferCount = 0;
    }

    void LightRenderer::RenderLightVolume( const Light * light, const Camera * camera )
    {
        return (this->*(m_renderFunc[light->m_type]))( light, camera );
    }

    void LightRenderer::RenderDirectional( const Light * light, const Camera * camera )
    {
        Matrix ori = RMatrix( light->m_orientation );

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

        Bool useFrontFaceMask = true;
        for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
        {
            SizeT j=0;
            for ( ; j<6; ++j )
            {
                F128 d;
                Store( d, Dot( points[i], planes[j] ) );

                if ( d[0] > ms_distanceThreshold )
                    break;
            }

            useFrontFaceMask = j != 6;
        }

        LightParameters params;
        params.m_lightMatrix        = Mul( camera->GetViewProjMatrix(), m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Zero4(), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( camera->GetViewMatrix(), light->m_position );
        params.m_direction          = TransformVector( camera->GetViewMatrix(), -ori.m_column[2] );
        params.m_spotParameters     = Zero4();

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programDirectional, &m_geomDirectional, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );
    }

    void LightRenderer::RenderOmni( const Light * light, const Camera * camera )
    {
        Matrix m = SMatrix( Splat( light->m_radius ) );
        m.m_column[3] = light->m_position;

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector points[5];
        points[0] = camera->m_position;
        points[1] = camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR];
        points[2] = camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR];
        points[3] = camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR];
        points[4] = camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR];

        Bool useFrontFaceMask = true;
        for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
        {
            Vector dist = Abs( points[i] - light->m_position );
            dist = Max( dist, Swizzle<1,2,0,3>(dist) );
            dist = Max( dist, Swizzle<2,0,1,3>(dist) );

            F128 d;
            Store( d, dist );

            useFrontFaceMask = d[0] > ( light->m_radius + ms_distanceThreshold );
        }

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        LightParameters params;
        params.m_lightMatrix        = Mul( camera->GetViewProjMatrix(), m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( camera->GetViewMatrix(), light->m_position );
        params.m_direction          = Zero4();
        params.m_spotParameters     = Zero4();

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programOmni, &m_geomOmni, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );
    }

    void LightRenderer::RenderSpot( const Light * light, const Camera * camera )
    {
        Matrix m = RMatrix( light->m_orientation );
        Vector lightDirection = -m.m_column[2];

        Vector points[5];
        points[0] = camera->m_position;
        points[1] = camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR];
        points[2] = camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR];
        points[3] = camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR];
        points[4] = camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR];

        RenderGeometry * geom;
        Bool useFrontFaceMask = true;
        if ( light->m_spotOutAngle < HalfPi() )
        {
            geom = &m_geomSpot;

            F32 tangent = Tan( 0.5f * light->m_spotOutAngle );
            Scale( m, Vector3( tangent, tangent, 1.0f ) * Splat(light->m_radius) );
            m.m_column[3] = light->m_position;

            // Check if camera position and frustum near corners are outside the light volume
            //
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

            for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
            {
                SizeT j=0;
                for ( ; j<5; ++j )
                {
                    F128 d;
                    Store( d, Dot( points[i], planes[j] ) );

                    if ( d[0] > ms_distanceThreshold )
                        break;
                }

                useFrontFaceMask = j != 5;
            }
        }
        else
        {
            geom = &m_geomDirectional;
            Matrix ori = m;

            F32 sinus = Sin( 0.5f * light->m_spotOutAngle ) * light->m_radius;
            Scale( m, Vector3( sinus, sinus, light->m_radius ) );
            m.m_column[3] = light->m_position;

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
            planes[0] = Select( normals[0], -(Splat( sinus )+Dot( normals[0], light->m_position )), Mask<0,0,0,1>() );
            planes[1] = Select( normals[1], -(Splat( sinus )+Dot( normals[1], light->m_position )), Mask<0,0,0,1>() );
            planes[2] = Select( normals[2], -(Splat( sinus )+Dot( normals[2], light->m_position )), Mask<0,0,0,1>() );
            planes[3] = Select( normals[3], -(Splat( sinus )+Dot( normals[3], light->m_position )), Mask<0,0,0,1>() );
            planes[4] = Select( normals[4], -(Splat( light->m_radius )+Dot( normals[4], light->m_position )), Mask<0,0,0,1>() );
            planes[5] = Select( normals[5], -(Dot( normals[5], light->m_position )), Mask<0,0,0,1>() );

            for ( SizeT i=0; useFrontFaceMask && i<5; ++i )
            {
                SizeT j=0;
                for ( ; j<6; ++j )
                {
                    F128 d;
                    Store( d, Dot( points[i], planes[j] ) );

                    if ( d[0] > ms_distanceThreshold )
                        break;
                }

                useFrontFaceMask = j != 6;
            }

        }

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        CARBON_ASSERT( light->m_spotInAngle >= 0.0 && light->m_spotInAngle <= Pi() );
        CARBON_ASSERT( light->m_spotOutAngle >= 0.0 && light->m_spotOutAngle <= Pi() );
        CARBON_ASSERT( light->m_spotInAngle < light->m_spotOutAngle );
        F32 spotCosIn = Cos( 0.5f * light->m_spotInAngle );
        F32 spotCosOut = Cos( 0.5f * light->m_spotOutAngle );

        LightParameters params;
        params.m_lightMatrix        = Mul( camera->GetViewProjMatrix(), m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( camera->GetViewMatrix(), light->m_position );
        params.m_direction          = TransformVector( camera->GetViewMatrix(), lightDirection );
        params.m_spotParameters     = Vector2( 1.0f, -spotCosOut ) / Splat( spotCosIn - spotCosOut );

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programSpot, geom, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );
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

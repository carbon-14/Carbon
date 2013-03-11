#include "Graphic/LightRenderer.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderCache.h"
#include "Graphic/Scene.h"
#include "Graphic/Camera.h"
#include "Graphic/QuadGeometry.h"
#include "Graphic/DebugRenderer.h"

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

    void LightRenderer::Initialize(  DebugRenderer * debugRenderer )
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

        U32 albedoLightingId        = ProgramCache::CreateId( "lightingAlbedo" );
        m_programAlbedo             = ProgramCache::GetProgram( albedoLightingId );

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

        m_stateAlbedoMask.m_colorWriteMask          = 0;
        m_stateAlbedoMask.m_enableDepthTest         = false;
        m_stateAlbedoMask.m_depthWriteMask          = false;
        m_stateAlbedoMask.m_enableStencilTest       = true;
        m_stateAlbedoMask.m_depthPass               = O_ZERO;

        m_stateAlbedoLighting.m_depthWriteMask      = false;
        m_stateAlbedoLighting.m_enableStencilTest   = true;
        m_stateAlbedoLighting.m_stencilRef          = 0;
        m_stateAlbedoLighting.m_stencilFunc         = F_EQUAL;
        m_stateAlbedoLighting.m_enableBlend         = true;
        m_stateAlbedoLighting.m_srcBlendFunc        = BF_ZERO;
        m_stateAlbedoLighting.m_dstBlendFunc        = BF_SRC_COLOR;
        m_stateAlbedoLighting.m_blendMode           = BM_ADD;

        m_uniformBufferCount = 0;

        m_debugRenderer = debugRenderer;
        m_debugDraw     = false;
    }

    void LightRenderer::Destroy()
    {
        m_debugRenderer = 0;

        UniformBufferArray::Iterator it = m_uniformBufferPool.Begin();
        UniformBufferArray::Iterator end = m_uniformBufferPool.End();
        for ( ; it!=end; ++it )
        {
            RenderDevice::DestroyBuffer( *it );
        }
        m_uniformBufferPool.Reserve(0);

        m_lights.Reserve(0);

        m_geomSpot.Destroy();
        m_geomOmni.Destroy();
        m_geomDirectional.Destroy();
    }

    void LightRenderer::Render( const Scene * scene, const Camera * camera )
    {
        const Matrix tr =
        {
            camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR],
            camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR],
            camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR],
            camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR]
        };

        Vector c = Select( camera->GetViewScaleNear(), Zero4(), Mask<0,0,0,1>() );
        Vector cos = Abs( Swizzle<2,2,2,2>(c) );

        const Vector distanceThreshold  = Splat( 0.5f ) * SquareLength(c) / cos;

        const Vector center             = camera->m_position - distanceThreshold * camera->GetInvViewMatrix().m_column[2];

        const Matrix& viewMatrix        = camera->GetViewMatrix();
        const Matrix& viewProjMatrix    = camera->GetViewProjMatrix();

        const Scene::LightArray& lights = scene->GetLights();
        Scene::LightArray::ConstIterator it = lights.Begin();
        Scene::LightArray::ConstIterator end = lights.End();
        for ( ; it != end; ++it )
        {
            RenderLightVolume( *it, viewMatrix, viewProjMatrix, center, distanceThreshold );
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

        ProgramCache::UseProgram( m_programAmbientMask );
        renderCache.SetRenderState( m_stateAlbedoMask );
        QuadGeometry::GetInstance().Draw();

        ProgramCache::UseProgram( m_programAmbientMask );
        renderCache.SetRenderState( m_stateAmbientMask );
        QuadGeometry::GetInstance().Draw();

        ProgramCache::UseProgram( m_programAlbedo );
        renderCache.SetRenderState( m_stateAlbedoLighting );
        QuadGeometry::GetInstance().Draw();

        m_lights.Clear();
        m_uniformBufferCount = 0;
    }

    void LightRenderer::SetDebugDraw( Bool enable )
    {
        m_debugDraw = enable;
    }

    Bool LightRenderer::GetDebugDraw() const
    {
        return m_debugDraw;
    }

    void LightRenderer::RenderLightVolume( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold )
    {
        F128 v;
        Store( v, light->m_value );
        if ( v[0] > 0.0f || v[1] > 0.0f || v[2] > 0.0f )
        {
            (this->*(m_renderFunc[light->m_type]))( light, viewMatrix, viewProjMatrix, center, distanceThreshold );
        }
    }

    void LightRenderer::RenderDirectional( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold )
    {
        Matrix ori = RMatrix( light->m_orientation );

        Matrix m = ori;
        Scale( m, Vector4( 0.5f*light->m_directionalWidth, 0.5f*light->m_directionalHeight, light->m_radius ) );
        m.m_column[3] = light->m_position;

        // Check if camera position and frustum near corners are outside the light volume
        //

        Matrix plane_matrix = Transpose( ori );
        plane_matrix.m_column[3] = Vector3( 0.0f, 0.0f, 0.5f*light->m_radius );

        Vector proj = Abs( TransformVertex( plane_matrix, center - light->m_position ) );
        Vector dist = distanceThreshold + Splat(0.5f) * Vector4( light->m_directionalWidth, light->m_directionalHeight, light->m_radius );

        Vector outside =  proj > dist;
        outside = Or( outside, Swizzle<1,2,0,3>( outside ) );
        outside = Or( outside, Swizzle<2,0,1,3>( outside ) );

        F128 c;
        Store(c,outside);

        Bool useFrontFaceMask = c[0] != 0.0f;

        LightParameters params;
        params.m_lightMatrix        = Mul( viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Zero4(), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( viewMatrix, light->m_position );
        params.m_direction          = TransformVector( viewMatrix, -ori.m_column[2] );
        params.m_spotParameters     = Zero4();

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programDirectional, &m_geomDirectional, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );

        if ( m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, -0.9f ) * Vector4( light->m_radius, light->m_radius, light->m_radius );
            ori.m_column[3] = light->m_position;

            Vector geom[4];
            geom[0] = TransformVertex( ori, Vector4( 0.0f, 0.0f, 0.0f ) );
            geom[1] = TransformVertex( ori, Vector4( 0.0f, 0.0f, -light->m_radius ) );
            geom[2] = TransformVertex( ori, scale );
            geom[3] = TransformVertex( ori, Swizzle<1,0,2,3>( scale ) );

            Vector color = One4();
            m_debugRenderer->RenderLine( geom[1], geom[0], color );
            m_debugRenderer->RenderLine( geom[1], geom[2], color );
            m_debugRenderer->RenderLine( geom[1], geom[3], color );
        }
    }

    void LightRenderer::RenderOmni( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold )
    {
        Matrix m = SMatrix( Splat( light->m_radius ) );
        m.m_column[3] = light->m_position;

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector dist = Abs( center - light->m_position );

        Vector threshold = distanceThreshold + Splat( light->m_radius );

        dist = dist > threshold;
        dist = Or( dist, Swizzle<1,2,0,3>(dist) );
        dist = Or( dist, Swizzle<2,0,1,3>(dist) );

        F128 d;
        Store(d,dist);

        Bool useFrontFaceMask = d[0] != 0.0f;

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        LightParameters params;
        params.m_lightMatrix        = Mul( viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( viewMatrix, light->m_position );
        params.m_direction          = Zero4();
        params.m_spotParameters     = Zero4();

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programOmni, &m_geomOmni, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );

        if ( m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, 0.0f );

            Vector geom[6];
            geom[0] = TransformVertex( m, Swizzle<0,2,2,3>( scale ) );
            geom[1] = TransformVertex( m, Swizzle<1,2,2,3>( scale ) );
            geom[2] = TransformVertex( m, Swizzle<2,0,2,3>( scale ) );
            geom[3] = TransformVertex( m, Swizzle<2,1,2,3>( scale ) );
            geom[4] = TransformVertex( m, Swizzle<2,2,0,3>( scale ) );
            geom[5] = TransformVertex( m, Swizzle<2,2,1,3>( scale ) );

            Vector color = One4();
            m_debugRenderer->RenderLine( geom[0], geom[1], color );
            m_debugRenderer->RenderLine( geom[2], geom[3], color );
            m_debugRenderer->RenderLine( geom[4], geom[5], color );
        }
    }

    void LightRenderer::RenderSpot( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold )
    {
        Matrix ori = RMatrix( light->m_orientation );

        Matrix m = ori;
        m.m_column[3] = light->m_position;

        RenderGeometry * geom;
        Vector outside;
        if ( light->m_spotOutAngle < HalfPi() )
        {
            geom = &m_geomSpot;

            F32 sinus = Sin( 0.5f * light->m_spotOutAngle );
            F32 cosinus = Cos( 0.5f * light->m_spotOutAngle );
            F32 tangent = sinus / cosinus;
            Scale( m, Vector3( tangent, tangent, 1.0f ) * Splat(light->m_radius) );

            // Check if camera position and frustum near corners are outside the light volume
            //

            Vector projX = Splat(cosinus) * ori.m_column[0];
            Vector projY = Splat(cosinus) * ori.m_column[1];
            Vector projZ = Splat(sinus) * ori.m_column[2];

            Matrix plane_matrix =
            {
                projZ - projX,
                projZ + projX,
                projZ - projY,
                projZ + projY,
            };
            plane_matrix = Transpose( plane_matrix );

            Vector pos = center - Select(light->m_position,Zero4(),Mask<0,0,0,1>());

            Vector proj = Dot( -ori.m_column[2], pos );

            outside = Mul( plane_matrix, pos ) > distanceThreshold;
            outside = Or( outside, proj > (distanceThreshold+Splat(light->m_radius)) );
            outside = Or( outside, Swizzle<1,0,3,2>( outside ) );
            outside = Or( outside, Swizzle<2,3,0,1>( outside ) );
        }
        else
        {
            geom = &m_geomDirectional;

            F32 sinus = Sin( 0.5f * light->m_spotOutAngle ) * light->m_radius;
            Scale( m, Vector3( sinus, sinus, light->m_radius ) );

            Matrix plane_matrix = Transpose( ori );
            plane_matrix.m_column[3] = Vector3( 0.0f, 0.0f, 0.5f*light->m_radius );

            Vector proj = Abs( TransformVertex( plane_matrix, center - light->m_position ) );
            Vector dist = distanceThreshold + Vector4( sinus, sinus, 0.5f*light->m_radius );

            outside =  proj > dist;
            outside = Or( outside, Swizzle<1,2,0,3>( outside ) );
            outside = Or( outside, Swizzle<2,0,1,3>( outside ) );
        }

        F128 c;
        Store(c,outside);

        Bool useFrontFaceMask = c[0] != 0.0f;

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        CARBON_ASSERT( light->m_spotInAngle >= 0.0 && light->m_spotInAngle <= Pi() );
        CARBON_ASSERT( light->m_spotOutAngle >= 0.0 && light->m_spotOutAngle <= Pi() );
        CARBON_ASSERT( light->m_spotInAngle < light->m_spotOutAngle );
        F32 spotCosIn = Cos( 0.5f * light->m_spotInAngle );
        F32 spotCosOut = Cos( 0.5f * light->m_spotOutAngle );

        LightParameters params;
        params.m_lightMatrix        = Mul( viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( viewMatrix, light->m_position );
        params.m_direction          = TransformVector( viewMatrix, -ori.m_column[2] );
        params.m_spotParameters     = Vector2( 1.0f, -spotCosOut ) / Splat( spotCosIn - spotCosOut );

        Handle uniformBuffer = GetUniformBuffer();
        void * data = RenderDevice::MapUniformBuffer( uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        RenderLight renderLight = { m_programSpot, geom, uniformBuffer, useFrontFaceMask };
        m_lights.PushBack( renderLight );

        if ( m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, -0.9f ) * Vector4( light->m_radius, light->m_radius, light->m_radius );
            ori.m_column[3] = light->m_position;

            Vector geom[4];
            geom[0] = TransformVertex( ori, Vector4( 0.0f, 0.0f, 0.0f ) );
            geom[1] = TransformVertex( ori, Vector4( 0.0f, 0.0f, -light->m_radius ) );
            geom[2] = TransformVertex( ori, scale );
            geom[3] = TransformVertex( ori, Swizzle<1,0,2,3>( scale ) );

            Vector color = One4();
            m_debugRenderer->RenderLine( geom[1], geom[0], color );
            m_debugRenderer->RenderLine( geom[1], geom[2], color );
            m_debugRenderer->RenderLine( geom[1], geom[3], color );
        }
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

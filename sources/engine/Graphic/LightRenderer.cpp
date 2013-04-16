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

        U32 envAmbientLightingId    = ProgramCache::CreateId( "lightingEnvAmbient" );
        m_programEnvAmbient         = ProgramCache::GetProgram( envAmbientLightingId );

        U32 albedoLightingId        = ProgramCache::CreateId( "lightingAlbedo" );
        m_programAlbedo             = ProgramCache::GetProgram( albedoLightingId );

        U32 materialLightingId      = ProgramCache::CreateId( "lightingMaterial" );
        m_programMaterial           = ProgramCache::GetProgram( materialLightingId );

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

        m_stateMaterialLighting.m_depthWriteMask      = false;
        m_stateMaterialLighting.m_enableStencilTest   = true;
        m_stateMaterialLighting.m_stencilRef          = 0;
        m_stateMaterialLighting.m_stencilFunc         = F_EQUAL;
        m_stateMaterialLighting.m_enableBlend         = true;
        m_stateMaterialLighting.m_srcBlendFunc        = BF_ONE;
        m_stateMaterialLighting.m_dstBlendFunc        = BF_ONE;
        m_stateMaterialLighting.m_blendMode           = BM_ADD;

        m_debugRenderer = debugRenderer;
    }

    void LightRenderer::Destroy()
    {
        m_debugRenderer = 0;

        m_geomSpot.Destroy();
        m_geomOmni.Destroy();
        m_geomDirectional.Destroy();
    }

    LightRenderer::Context * LightRenderer::CreateContext( DebugRenderer::Context * debugContext )
    {
        Context * context = MemoryManager::New< Context >();

        context->m_renderLights         = (RenderLight*)MemoryManager::Malloc( 16 * sizeof(RenderLight), MemoryUtils::AlignOf<RenderLight>() );

        RenderLight * it                = context->m_renderLights;
        const RenderLight * end         = it + 16;
        for ( ; it!=end; ++it )
        {
            it->m_uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(LightParameters), 0, BU_DYNAMIC );
        }

        context->m_renderLightSize      = 16;
        context->m_renderLightCount     = 0;

        context->m_linearDepthTexture   = 0;
        context->m_normalTexture        = 0;
        context->m_colorTexture         = 0;

        context->m_debugContext         = debugContext;
        context->m_debugDraw            = false;

        return context;
    }

    void LightRenderer::UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, Handle linearDepthTexture, Handle normalTexture, Handle colorTexture, Handle envTexture )
    {
        FrustumQuadTree& quadTree = context->m_quadTree;
        if ( quadTree.GetWidth() != width || quadTree.GetHeight() != height || quadTree.GetCamera() != camera )
        {
            quadTree.Clear();
            quadTree.Build( 16, width, height, camera );
        }

        const Matrix tr =
        {
            camera->GetFrustum().GetCorners()[FC_LEFT_BOTTOM_NEAR],
            camera->GetFrustum().GetCorners()[FC_RIGHT_BOTTOM_NEAR],
            camera->GetFrustum().GetCorners()[FC_LEFT_TOP_NEAR],
            camera->GetFrustum().GetCorners()[FC_RIGHT_TOP_NEAR]
        };

        Vector c = Select( camera->GetViewScaleNear(), Zero4, Mask<0,0,0,1>() );
        Vector cos = Abs( Swizzle<2,2,2,2>(c) );

        context->m_renderLightCount     = 0;
        context->m_colSphereRadius      = Splat( 0.5f ) * SquareLength(c) / cos;
        context->m_colSphereCenter      = camera->m_position - context->m_colSphereRadius * camera->GetInvViewMatrix().m_column[2];
        context->m_viewMatrix           = camera->GetViewMatrix();
        context->m_viewProjMatrix       = camera->GetViewProjMatrix();
        context->m_linearDepthTexture   = linearDepthTexture;
        context->m_normalTexture        = normalTexture;
        context->m_colorTexture         = colorTexture;
        context->m_envTexture           = envTexture;
    }

    void LightRenderer::DestroyContext( Context * context )
    {
        RenderLight * it = context->m_renderLights;
        const RenderLight * end = it + context->m_renderLightSize;
        for ( ; it!=end; ++it )
        {
            RenderDevice::DestroyBuffer( it->m_uniformBuffer );
        }
        MemoryManager::Free( context->m_renderLights );

        context->m_quadTree.Clear();

        MemoryManager::Delete( context );
    }

    void LightRenderer::Render( const Light * light, Context * context ) const
    {
        (this->*(m_renderFunc[light->m_type]))( light, context );
    }

    void LightRenderer::Draw( Context * context, RenderCache& renderCache ) const
    {
        RenderDevice::BindTexture( context->m_linearDepthTexture    , 0 );
        RenderDevice::BindTexture( context->m_normalTexture         , 1 );
        RenderDevice::BindTexture( context->m_colorTexture          , 2 );
        RenderDevice::BindTextureCube( context->m_envTexture        , 3 );

        renderCache.SetClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        RenderDevice::Clear( RM_COLOR );

        renderCache.SetSRGBWrite( false );

        ProgramCache::UseProgram( m_programAmbientMask );
        renderCache.SetRenderState( m_stateAmbientMask );
        QuadGeometry::GetInstance().Draw();

        ProgramCache::UseProgram( m_programAmbient );
        renderCache.SetRenderState( m_stateAmbientLighting );
        QuadGeometry::GetInstance().Draw();

        const RenderLight * it = context->m_renderLights;
        const RenderLight * end = it + context->m_renderLightCount;
        for ( ; it!=end; ++it )
        {
            RenderDevice::BindUniformBuffer( it->m_uniformBuffer, 0 );

            // Stencil masking
            ProgramCache::UseProgram( m_programMask );

            if ( it->m_useFrontFace )
            {
                renderCache.SetRenderState( m_stateMaskClear0 );
                 it->m_geometry->Draw();
        
                renderCache.SetRenderState( m_stateMaskFront );
                 it->m_geometry->Draw();
            }
            else
            {
                renderCache.SetRenderState( m_stateMaskClear1 );
                it->m_geometry->Draw();
            }

            renderCache.SetRenderState( m_stateMaskBack );
            it->m_geometry->Draw();

            // Lighting 
            ProgramCache::UseProgram( it->m_program );

            renderCache.SetRenderState( m_stateLighting );
            it->m_geometry->Draw();
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

        /*ProgramCache::UseProgram( m_programMaterial );
        renderCache.SetRenderState( m_stateMaterialLighting );
        QuadGeometry::GetInstance().Draw();*/
    }

    void LightRenderer::DrawEnv( Context * context, RenderCache& renderCache ) const
    {
        RenderDevice::BindTexture( context->m_linearDepthTexture    , 0 );
        RenderDevice::BindTexture( context->m_normalTexture         , 1 );
        RenderDevice::BindTexture( context->m_colorTexture          , 2 );

        renderCache.SetClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        RenderDevice::Clear( RM_COLOR );

        renderCache.SetSRGBWrite( false );

        ProgramCache::UseProgram( m_programAmbientMask );
        renderCache.SetRenderState( m_stateAmbientMask );
        QuadGeometry::GetInstance().Draw();

        ProgramCache::UseProgram( m_programEnvAmbient );
        renderCache.SetRenderState( m_stateAmbientLighting );
        QuadGeometry::GetInstance().Draw();

        const RenderLight * it = context->m_renderLights;
        const RenderLight * end = it + context->m_renderLightCount;
        for ( ; it!=end; ++it )
        {
            RenderDevice::BindUniformBuffer( it->m_uniformBuffer, 0 );

            // Stencil masking
            ProgramCache::UseProgram( m_programMask );

            if ( it->m_useFrontFace )
            {
                renderCache.SetRenderState( m_stateMaskClear0 );
                 it->m_geometry->Draw();
        
                renderCache.SetRenderState( m_stateMaskFront );
                 it->m_geometry->Draw();
            }
            else
            {
                renderCache.SetRenderState( m_stateMaskClear1 );
                it->m_geometry->Draw();
            }

            renderCache.SetRenderState( m_stateMaskBack );
            it->m_geometry->Draw();

            // Lighting 
            ProgramCache::UseProgram( it->m_program );

            renderCache.SetRenderState( m_stateLighting );
            it->m_geometry->Draw();
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
    }

    void LightRenderer::RenderDirectional( const Light * light, Context * context ) const
    {
        RenderLight * render_light = GetRenderLight( context );
        render_light->m_program  = m_programDirectional;
        render_light->m_geometry = &m_geomDirectional;

        Matrix ori = RMatrix( light->m_orientation );

        Matrix m = ori;
        Scale( m, Vector4( 0.5f*light->m_directionalWidth, 0.5f*light->m_directionalHeight, light->m_radius ) );
        m.m_column[3] = light->m_position;

        LightParameters params;
        params.m_lightMatrix        = Mul( context->m_viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Zero4, Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( context->m_viewMatrix, light->m_position );
        params.m_direction          = TransformVector( context->m_viewMatrix, -ori.m_column[2] );
        params.m_spotParameters     = Zero4;

        void * data = RenderDevice::MapUniformBuffer( render_light->m_uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        // Check if camera position and frustum near corners are outside the light volume
        //

        Matrix plane_matrix = Transpose( ori );
        plane_matrix.m_column[3] = Vector3( 0.0f, 0.0f, 0.5f*light->m_radius );

        Vector proj = Abs( TransformVertex( plane_matrix, context->m_colSphereCenter - light->m_position ) );
        Vector dist = context->m_colSphereRadius + Splat(0.5f) * Vector4( light->m_directionalWidth, light->m_directionalHeight, light->m_radius );

        Vector outside =  proj > dist;
        outside = Or( outside, Swizzle<1,2,0,3>( outside ) );
        outside = Or( outside, Swizzle<2,0,1,3>( outside ) );

        F128 c;
        Store(c,outside);

        render_light->m_useFrontFace = c[0] != 0.0f;

        if ( context->m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, -0.9f ) * Vector4( light->m_radius, light->m_radius, light->m_radius );
            ori.m_column[3] = light->m_position;

            Vector geom[4];
            geom[0] = TransformVertex( ori, Vector4( 0.0f, 0.0f, 0.0f ) );
            geom[1] = TransformVertex( ori, Vector4( 0.0f, 0.0f, -light->m_radius ) );
            geom[2] = TransformVertex( ori, scale );
            geom[3] = TransformVertex( ori, Swizzle<1,0,2,3>( scale ) );

            Vector color = One4;
            DebugLine a = { geom[1], geom[0], color };
            DebugLine b = { geom[1], geom[2], color };
            DebugLine c = { geom[1], geom[3], color };

            m_debugRenderer->Render( a, context->m_debugContext );
            m_debugRenderer->Render( b, context->m_debugContext );
            m_debugRenderer->Render( c, context->m_debugContext );
        }
    }

    void LightRenderer::RenderOmni( const Light * light, Context * context ) const
    {
        RenderLight * render_light = GetRenderLight( context );
        render_light->m_program  = m_programOmni;
        render_light->m_geometry = &m_geomOmni;

        Matrix m = SMatrix( Splat( light->m_radius ) );
        m.m_column[3] = light->m_position;

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        LightParameters params;
        params.m_lightMatrix        = Mul( context->m_viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( context->m_viewMatrix, light->m_position );
        params.m_direction          = Zero4;
        params.m_spotParameters     = Zero4;

        void * data = RenderDevice::MapUniformBuffer( render_light->m_uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        // Check if camera position and frustum near corners are outside the light volume
        //
        Vector dist = Abs( context->m_colSphereCenter - light->m_position );

        Vector threshold = context->m_colSphereRadius + Splat( light->m_radius );

        dist = dist > threshold;
        dist = Or( dist, Swizzle<1,2,0,3>(dist) );
        dist = Or( dist, Swizzle<2,0,1,3>(dist) );

        F128 d;
        Store(d,dist);

        render_light->m_useFrontFace = d[0] != 0.0f;

        if ( context->m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, 0.0f );

            Vector geom[6];
            geom[0] = TransformVertex( m, Swizzle<0,2,2,3>( scale ) );
            geom[1] = TransformVertex( m, Swizzle<1,2,2,3>( scale ) );
            geom[2] = TransformVertex( m, Swizzle<2,0,2,3>( scale ) );
            geom[3] = TransformVertex( m, Swizzle<2,1,2,3>( scale ) );
            geom[4] = TransformVertex( m, Swizzle<2,2,0,3>( scale ) );
            geom[5] = TransformVertex( m, Swizzle<2,2,1,3>( scale ) );

            Vector color = One4;
            DebugLine a = { geom[0], geom[1], color };
            DebugLine b = { geom[2], geom[3], color };
            DebugLine c = { geom[4], geom[5], color };

            m_debugRenderer->Render( a, context->m_debugContext );
            m_debugRenderer->Render( b, context->m_debugContext );
            m_debugRenderer->Render( c, context->m_debugContext );
        }
    }

    void LightRenderer::RenderSpot( const Light * light, Context * context ) const
    {
        RenderLight * render_light = GetRenderLight( context );
        render_light->m_program  = m_programSpot;

        Matrix ori = RMatrix( light->m_orientation );

        Matrix m = ori;
        m.m_column[3] = light->m_position;

        CARBON_ASSERT( light->m_radius > 0 );
        F32 invSqrRadius = 1.0f / ( light->m_radius * light->m_radius );

        CARBON_ASSERT( light->m_spotInAngle >= 0.0 && light->m_spotInAngle <= Pi );
        CARBON_ASSERT( light->m_spotOutAngle >= 0.0 && light->m_spotOutAngle <= Pi );
        CARBON_ASSERT( light->m_spotInAngle < light->m_spotOutAngle );
        F32 spotCosIn = Cos( 0.5f * light->m_spotInAngle );
        F32 spotCosOut = Cos( 0.5f * light->m_spotOutAngle );

        Vector outside;
        if ( light->m_spotOutAngle < HalfPi )
        {
            render_light->m_geometry = &m_geomSpot;

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

            Vector pos = context->m_colSphereCenter - Select(light->m_position,Zero4,Mask<0,0,0,1>());

            Vector proj = Dot( -ori.m_column[2], pos );

            outside = Mul( plane_matrix, pos ) > context->m_colSphereRadius;
            outside = Or( outside, proj > (context->m_colSphereRadius+Splat(light->m_radius)) );
            outside = Or( outside, Swizzle<1,0,3,2>( outside ) );
            outside = Or( outside, Swizzle<2,3,0,1>( outside ) );
        }
        else
        {
            render_light->m_geometry = &m_geomDirectional;

            F32 sinus = Sin( 0.5f * light->m_spotOutAngle ) * light->m_radius;
            Scale( m, Vector3( sinus, sinus, light->m_radius ) );

            Matrix plane_matrix = Transpose( ori );
            plane_matrix.m_column[3] = Vector3( 0.0f, 0.0f, 0.5f*light->m_radius );

            Vector proj = Abs( TransformVertex( plane_matrix, context->m_colSphereCenter - light->m_position ) );
            Vector dist = context->m_colSphereRadius + Vector4( sinus, sinus, 0.5f*light->m_radius );

            outside =  proj > dist;
            outside = Or( outside, Swizzle<1,2,0,3>( outside ) );
            outside = Or( outside, Swizzle<2,0,1,3>( outside ) );
        }

        F128 c;
        Store(c,outside);

        render_light->m_useFrontFace = c[0] != 0.0f;

        LightParameters params;
        params.m_lightMatrix        = Mul( context->m_viewProjMatrix, m );
        params.m_valueInvSqrRadius  = Select( light->m_value, Splat( invSqrRadius ), Mask<0,0,0,1>() );
        params.m_position           = TransformVertex( context->m_viewMatrix, light->m_position );
        params.m_direction          = TransformVector( context->m_viewMatrix, -ori.m_column[2] );
        params.m_spotParameters     = Vector2( 1.0f, -spotCosOut ) / Splat( spotCosIn - spotCosOut );

        void * data = RenderDevice::MapUniformBuffer( render_light->m_uniformBuffer, BA_WRITE_ONLY );
        MemoryUtils::MemCpy( data, &params, sizeof(LightParameters) );
        RenderDevice::UnmapUniformBuffer();

        if ( context->m_debugDraw )
        {
            Vector scale = Vector4( -0.05f, +0.05f, -0.9f ) * Vector4( light->m_radius, light->m_radius, light->m_radius );
            ori.m_column[3] = light->m_position;

            Vector geom[4];
            geom[0] = TransformVertex( ori, Vector4( 0.0f, 0.0f, 0.0f ) );
            geom[1] = TransformVertex( ori, Vector4( 0.0f, 0.0f, -light->m_radius ) );
            geom[2] = TransformVertex( ori, scale );
            geom[3] = TransformVertex( ori, Swizzle<1,0,2,3>( scale ) );

            Vector color = One4;
            DebugLine a = { geom[1], geom[0], color };
            DebugLine b = { geom[1], geom[2], color };
            DebugLine c = { geom[1], geom[3], color };

            m_debugRenderer->Render( a, context->m_debugContext );
            m_debugRenderer->Render( b, context->m_debugContext );
            m_debugRenderer->Render( c, context->m_debugContext );
        }
    }

    RenderLight * LightRenderer::GetRenderLight( Context * context ) const
    {
        if ( context->m_renderLightCount == context->m_renderLightSize )
        {
            context->m_renderLightSize = 2 * context->m_renderLightSize;

            RenderLight * render_lights = (RenderLight*)MemoryManager::Malloc( context->m_renderLightSize * sizeof(RenderLight), MemoryUtils::AlignOf<RenderLight>() );
            MemoryUtils::MemCpy( render_lights, context->m_renderLights, context->m_renderLightCount * sizeof(RenderLight) );
            MemoryManager::Free( context->m_renderLights );
            context->m_renderLights = render_lights;

            RenderLight * it = render_lights + context->m_renderLightCount;
            const RenderLight * end = it + context->m_renderLightCount;
            for ( ; it!=end; ++it )
            {
                it->m_uniformBuffer = RenderDevice::CreateUniformBuffer( sizeof(LightParameters), 0, BU_DYNAMIC );
            }
        }

        return context->m_renderLights + context->m_renderLightCount++;
    }
}

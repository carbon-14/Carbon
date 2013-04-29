#pragma once
#ifndef _GRAPHIC_LIGHTRENDERER_H
#define _GRAPHIC_LIGHTRENDERER_H

#include "Graphic/RenderState.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/Light.h"
#include "Graphic/LightGeometry.h"
#include "Graphic/DebugRenderer.h"

#include "Core/Matrix.h"

namespace Graphic
{
    class DebugRenderer;
    class RenderCache;
    class Scene;
    class Camera;

    struct _GraphicExport LightParameters
    {
        Matrix  m_lightMatrix;
        Vector  m_valueInvSqrRadius;
        Vector  m_position;
        Vector  m_direction;
        Vector  m_spotParameters;
    };

    class _GraphicExport LightRenderer
    {
    public:
        struct Context
        {
            RenderLight *               m_renderLights;
            SizeT                       m_renderLightSize;
            SizeT                       m_renderLightCount;

            Vector                      m_colSphereRadius;
            Vector                      m_colSphereCenter;
            Matrix                      m_viewMatrix;
            Matrix                      m_viewProjMatrix;

            Handle                      m_linearDepthTexture;
            Handle                      m_normalTexture;
            Handle                      m_colorTexture;
            Handle                      m_envTexture;

            DebugRenderer::Context *    m_debugContext;
            Bool                        m_debugDraw;
        };

    public:
        void Initialize( DebugRenderer * debugRenderer );
        void Destroy();

        static Context * CreateContext( DebugRenderer::Context * context );
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, Handle linearDepthTexture, Handle normalTexture, Handle colorTexture, Handle envTexture );
        static void DestroyContext( Context * context );

        void Render( const Light * const * lights, SizeT lightCount, Context * context ) const;
        void Draw( const Context * context, RenderCache& renderCache ) const;
        void DrawEnv( const Context * context, RenderCache& renderCache ) const;

    private:
        void RenderDirectional( const Light * lights, Context * context ) const;
        void RenderOmni( const Light * lights, Context * context ) const;
        void RenderSpot( const Light * lights, Context * context ) const;

        RenderLight * GetRenderLight( Context * context ) const;

    private:
        typedef void (Graphic::LightRenderer::*RenderFunc)( const Light *, Context * ) const;

        RenderFunc          m_renderFunc[LT_COUNT];

        DirectionalGeometry m_geomDirectional;
        OmniGeometry        m_geomOmni;
        SpotGeometry        m_geomSpot;

        ProgramHandle       m_programMask;
        ProgramHandle       m_programAmbientMask;
        ProgramHandle       m_programAmbient;
        ProgramHandle       m_programEnvAmbient;
        ProgramHandle       m_programDirectional;
        ProgramHandle       m_programOmni;
        ProgramHandle       m_programSpot;
        ProgramHandle       m_programAlbedo;
        ProgramHandle       m_programMaterial;

        RenderState         m_stateMaskClear0;
        RenderState         m_stateMaskClear1;
        RenderState         m_stateMaskFront;
        RenderState         m_stateMaskBack;
        RenderState         m_stateLighting;
        RenderState         m_stateAmbientMask;
        RenderState         m_stateAmbientLighting;
        RenderState         m_stateAlbedoMask;
        RenderState         m_stateAlbedoLighting;
        RenderState         m_stateMaterialLighting;

        DebugRenderer *     m_debugRenderer;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::LightParameters );

#endif // _GRAPHIC_LIGHTRENDERER

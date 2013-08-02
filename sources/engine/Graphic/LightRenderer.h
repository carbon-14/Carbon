#pragma once
#ifndef _GRAPHIC_LIGHTRENDERER_H
#define _GRAPHIC_LIGHTRENDERER_H

#include "Graphic/RenderState.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/Light.h"
#include "Graphic/LightGeometry.h"
#include "Graphic/DebugRenderer.h"
#include "Graphic/Rasterizer.h"

namespace Graphic
{
    class Scene;
    class Camera;

    struct LightInfos
    {
        Vector  m_valueInvSqrRadius;
        Vector  m_position;
        Vector  m_direction;
        Vector  m_spotParameters;
        U32     m_depth;
        U32     m_flags;
        U8      m_quad[4];
    };

    struct LightingParameters
    {
        U32     m_lightCount;
    };

    class _GraphicExport LightRenderer
    {
    public:
        struct Context
        {
            SizeT                       m_tileWidth;
            SizeT                       m_tileHeight;
            SizeT                       m_lightBufferSize;
            Handle                      m_lightBuffer;
            Handle                      m_lightingParameters;

            F32                         m_cameraZFar;
            Matrix                      m_viewMatrix;
            Matrix                      m_viewProjMatrix;

            Handle                      m_tiledDepthBuffer;
            Handle                      m_depthTexture;
            Handle                      m_normalTexture;
            Handle                      m_colorTexture;
            Handle                      m_envSharpTexture;
            Handle                      m_envBlurTexture;
            Handle                      m_lightTexture;

            Rasterizer::Context *       m_rasterizerContext;

            DebugRenderer::Context *    m_debugContext;
            Bool                        m_debugDraw;
        };

    public:
        void Initialize( Rasterizer * rasterizer, DebugRenderer * debugRenderer );
        void Destroy();

        static Context * CreateContext( Rasterizer::Context * rasterizerContext, DebugRenderer::Context * context );
        static void UpdateContext( Context * context, SizeT width, SizeT height, const Camera * camera, SizeT tileWidth, SizeT tileHeight, Handle tiledDepthBuffer, Handle depthTexture, Handle normalTexture, Handle colorTexture, Handle envSharpTexture, Handle envBlurTexture, Handle lightTexture );
        static void DestroyContext( Context * context );

        void Render( const Light * const * lights, SizeT lightCount, Context * context ) const;
        void Draw( const Context * context, RenderCache& renderCache ) const;
        void DrawEnv( const Context * context, CubeFace face, RenderCache& renderCache ) const;

    private:
        void RenderDirectional( const Light * lights, LightInfos * params, Context * context ) const;
        void RenderOmni( const Light * lights, LightInfos * params, Context * context ) const;
        void RenderSpot( const Light * lights, LightInfos * params, Context * context ) const;

        //RenderLight * GetRenderLight( Context * context ) const;

    private:
        typedef void (Graphic::LightRenderer::*RenderFunc)( const Light *, LightInfos * params, Context * ) const;

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
        ProgramHandle       m_programTileLighting;
        ProgramHandle       m_programTileEnvLighting;

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

        Rasterizer *        m_rasterizer;
        DebugRenderer *     m_debugRenderer;
    };
}

#endif // _GRAPHIC_LIGHTRENDERER

#pragma once
#ifndef _GRAPHIC_LIGHTRENDERER_H
#define _GRAPHIC_LIGHTRENDERER_H

#include "Graphic/RenderState.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/Light.h"
#include "Graphic/LightGeometry.h"

#include "Core/Matrix.h"

namespace Graphic
{
    class DebugRenderer;
    class RenderCache;
    class RenderGeometry;
    class Scene;
    class Camera;

    class _GraphicExport LightRenderer
    {
    public:
        struct RenderLight
        {
            ProgramHandle       m_program;
            RenderGeometry *    m_geometry;
            Handle              m_uniformBuffer;
            Bool                m_useFrontFace;
        };

    public:
        void Initialize( DebugRenderer * debugRenderer );
        void Destroy();

        void Render( const Scene * scene, const Camera * camera );
        void Draw( RenderCache& renderCache, Handle frameParameters, Handle depthStencilTexture, Handle normalTexture, Handle colorTexture );

        void SetDebugDraw( Bool enable );
        Bool GetDebugDraw() const;

    private:
        void RenderLightVolume( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold );
        void RenderDirectional( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold );
        void RenderOmni( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold );
        void RenderSpot( const Light * light, const Matrix& viewMatrix, const Matrix& viewProjMatrix, const Vector& center, const Vector& distanceThreshold );

        Handle GetUniformBuffer();

    private:
        typedef Array< RenderLight > LightArray;
        typedef Array< Handle > UniformBufferArray;
        typedef void (Graphic::LightRenderer::*RenderFunc)( const Light *, const Matrix&, const Matrix&, const Vector&, const Vector& );

        LightArray          m_lights;
        UniformBufferArray  m_uniformBufferPool;
        SizeT               m_uniformBufferCount;

        RenderFunc          m_renderFunc[LT_COUNT];

        DirectionalGeometry m_geomDirectional;
        OmniGeometry        m_geomOmni;
        SpotGeometry        m_geomSpot;

        ProgramHandle       m_programMask;
        ProgramHandle       m_programAmbientMask;
        ProgramHandle       m_programAmbient;
        ProgramHandle       m_programDirectional;
        ProgramHandle       m_programOmni;
        ProgramHandle       m_programSpot;
        ProgramHandle       m_programAlbedo;

        RenderState         m_stateMaskClear0;
        RenderState         m_stateMaskClear1;
        RenderState         m_stateMaskFront;
        RenderState         m_stateMaskBack;
        RenderState         m_stateLighting;
        RenderState         m_stateAmbientMask;
        RenderState         m_stateAmbientLighting;
        RenderState         m_stateAlbedoMask;
        RenderState         m_stateAlbedoLighting;

        DebugRenderer *     m_debugRenderer;
        Bool                m_debugDraw;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::LightRenderer::RenderLight );

#endif // _GRAPHIC_LIGHTRENDERER

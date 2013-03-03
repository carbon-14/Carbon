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
    class RenderCache;
    class RenderGeometry;
    class Scene;
    class Camera;

    class _GraphicExport LightRenderer
    {
    public:
        struct RenderLight
        {
            LightType   m_type;
            Handle      m_uniformBuffer;
            Bool        m_useFrontFace;
        };

    public:
        void Initialize();
        void Destroy();

        void Render( const Scene * scene, const Camera * camera );
        void Draw( RenderCache& renderCache, Handle frameParameters, Handle depthStencilTexture, Handle normalTexture, Handle colorTexture );

    private:
        Matrix ComputeLightTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const;
        Matrix DirectionalTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const;
        Matrix OmniTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const;
        Matrix SpotTransform( const Light * light, const Camera * camera, Vector& lightDirection, Bool& useFrontFaceMask ) const;

        Handle GetUniformBuffer();

    private:
        typedef Array< RenderLight > LightArray;
        typedef Array< Handle > UniformBufferArray;
        typedef Matrix (Graphic::LightRenderer::*TransformFunc)( const Light *, const Camera *, Vector&, Bool& ) const;

        struct RenderConfig
        {
            ProgramHandle       m_program;
            RenderGeometry *    m_geometry;
            TransformFunc       m_func;
        };

        LightArray          m_lights;
        UniformBufferArray  m_uniformBufferPool;
        SizeT               m_uniformBufferCount;

        DirectionalGeometry m_directionalGeometry;
        OmniGeometry        m_omniGeometry;
        SpotGeometry        m_spotGeometry;

        RenderConfig        m_configs[LT_COUNT];
        ProgramHandle       m_mask;
        RenderState         m_maskClear0;
        RenderState         m_maskClear1;
        RenderState         m_maskFront;
        RenderState         m_maskBack;
        RenderState         m_lighting;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::LightRenderer::RenderLight );

#endif // _GRAPHIC_LIGHTRENDERER

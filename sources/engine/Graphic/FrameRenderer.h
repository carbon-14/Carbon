#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/RenderList.h"
#include "Graphic/DebugRenderer.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"

namespace Graphic
{
    class Scene;
    class Camera;

    struct _GraphicExport FrameParameters
    {
        Vector  m_viewportSize;
        Vector  m_depthRange;
        Vector  m_viewScale;
        Vector  m_ambientSkyLight;
        Vector  m_ambientGroundLight;
        Matrix  m_viewMatrix;
        Matrix  m_projMatrix;
        Matrix  m_viewProjMatrix;
    };

    class _GraphicExport FrameRenderer
    {
    public:
        struct Context
        {
            Camera *                m_camera;
            Scene *                 m_scene;

            SizeT                   m_width;
            SizeT                   m_height;
            Handle                  m_depthStencilTexture;
            Handle                  m_normalTexture;
            Handle                  m_colorTexture;
            Handle                  m_linearDepthTexture;
            Handle                  m_lightTexture;
            Handle                  m_finalColorBuffer;

            Handle                  m_geomFramebuffer;
            Handle                  m_linearizeDepthFramebuffer;
            Handle                  m_lightFramebuffer;
            Handle                  m_finalFramebuffer;

            Handle                  m_uniformBuffer;
            RenderList              m_opaqueList;

            DebugRenderer::Context *m_debugRendererContext;
            MeshRenderer::Context * m_meshRendererContext;
            LightRenderer::Context *m_lightRendererContext;
        };

    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer );
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, Camera * camera, Scene * scene );
        static void DestroyContext( Context * context );

        void Render( Context * context ) const;
        void Draw( Context * context, RenderCache& renderCache ) const;

    private:
        void LinearizeDepth( Context * context, RenderCache& renderCache ) const;
        void ApplyToneMapping( Context * context, RenderCache& renderCache ) const;

        DebugRenderer * m_debugRenderer;
        MeshRenderer *  m_meshRenderer;
        LightRenderer * m_lightRenderer;

        ProgramHandle   m_programLinearDepth;
        ProgramHandle   m_programToneMapping;

        RenderState     m_renderStateLinearDepth;
        RenderState     m_renderStateToneMapping;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::FrameParameters );

#endif // _GRAPHIC_FRAMERENDERER_H

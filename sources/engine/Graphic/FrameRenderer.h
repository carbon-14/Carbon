#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/RenderList.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"
#include "Graphic/DebugRenderer.h"

namespace Graphic
{
    class Scene;
    class Camera;

    class _GraphicExport FrameRenderer
    {
    public:
        struct Context
        {
            SizeT       m_width;
            SizeT       m_height;
            Camera *    m_camera;
            Scene *     m_scene;

            Handle      m_depthStencilTexture;
            Handle      m_normalTexture;
            Handle      m_colorTexture;
            Handle      m_linearDepthTexture;
            Handle      m_lightTexture;
            Handle      m_finalColorBuffer;

            Handle      m_geomFramebuffer;
            Handle      m_linearizeDepthFramebuffer;
            Handle      m_lightFramebuffer;
            Handle      m_finalFramebuffer;
        };

    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize();
        void Destroy();

        void Render( const Context& context );

        static void InitializeFrameContext( Context& context, SizeT width, SizeT height, Camera * camera, Scene * scene );
        static void DestroyFrameContext( Context& context );

        void SetLightDebugDraw( Bool enable );
        Bool GetLightDebugDraw() const;

        void RenderDebugLine( const Vector& position0, const Vector& position1, const Vector& color );

    private:
        void UpdateFrameUniformBuffer( const Context& context );
        void LinearizeDepth( Handle depthStencilTexture );
        void ApplyToneMapping( Handle lightTexture );

        RenderCache     m_renderCache;
        RenderList      m_opaqueList;

        MeshRenderer    m_meshRenderer;
        LightRenderer   m_lightRenderer;
        DebugRenderer   m_debugRenderer;

        ProgramHandle   m_programLinearDepth;
        ProgramHandle   m_programToneMapping;

        RenderState     m_renderStateLinearDepth;
        RenderState     m_renderStateToneMapping;

        Handle          m_frameUniformBuffer;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::FrameRenderer::Context );

#endif // _GRAPHIC_FRAMERENDERER_H

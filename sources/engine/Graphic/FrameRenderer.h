#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/DLL.h"

#include "Core/Matrix.h"

#include "Graphic/RenderList.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"
#include "Graphic/DebugRenderer.h"

namespace Graphic
{
    class Scene;
    class Camera;

    struct FrameContext
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

    class _GraphicExport FrameRenderer
    {
    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize();
        void Destroy();

        void Render( const FrameContext& context );

        static void InitializeFrameContext( FrameContext& context, SizeT width, SizeT height, Camera * camera, Scene * scene );
        static void DestroyFrameContext( FrameContext& context );

        void SetLightDebugDraw( Bool enable );
        Bool GetLightDebugDraw() const;

        void RenderDebugLine( const Vector& position0, const Vector& position1, const Vector& color );

    private:
        void UpdateFrameUniformBuffer( const FrameContext& context );
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

CARBON_DECLARE_POD_TYPE( Graphic::FrameContext );

#endif // _GRAPHIC_FRAMERENDERER_H

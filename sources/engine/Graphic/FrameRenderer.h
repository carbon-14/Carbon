#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/DLL.h"

#include "Core/Matrix.h"

#include "Graphic/RenderList.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"

namespace Graphic
{
    class Scene;
    class Camera;

    struct FrameContext
    {
        SizeT       m_width;
        SizeT       m_height;
        Camera *    m_camera;

        Handle      m_depthStencilTexture;
        Handle      m_normalTexture;
        Handle      m_colorTexture;
        Handle      m_linearDepthTexture;
        Handle      m_lightTexture;

        Handle      m_geomFramebuffer;
        Handle      m_linearizeDepthFramebuffer;
        Handle      m_lightFramebuffer;
    };

    class _GraphicExport FrameRenderer
    {
    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize();
        void Destroy();

        void Render( const Scene * scene, const FrameContext& context );

        static void InitializeFrameContext( FrameContext& context, SizeT width, SizeT height, Camera * camera );
        static void DestroyFrameContext( FrameContext& context );

    private:
        void UpdateFrameUniformBuffer( const FrameContext& context );
        void LinearizeDepth( Handle depthStencilTexture );
        void ApplyToneMapping( Handle lightTexture );

        RenderCache     m_renderCache;
        RenderList      m_opaqueList;

        MeshRenderer    m_meshRenderer;
        LightRenderer   m_lightRenderer;

        ProgramHandle   m_programLinearDepth;
        ProgramHandle   m_programToneMapping;

        RenderState     m_renderStateLinearDepth;
        RenderState     m_renderStateToneMapping;

        Handle          m_frameUniformBuffer;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::FrameContext );

#endif // _GRAPHIC_FRAMERENDERER_H

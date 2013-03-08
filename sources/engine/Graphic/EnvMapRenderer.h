#pragma once
#ifndef _GRAPHIC_ENVMAPRENDERER_H
#define _GRAPHIC_ENVMAPRENDERER_H

#include "Graphic/Types.h"

namespace Graphic
{
    class Camera;
    class Scene;
    class DebugRenderer;

    class _Graphic EnvMapRenderer
    {
    public:
        struct Context
        {
            SizeT       m_size;
            Camera *    m_camera;
            Scene *     m_scene;

            Handle      m_depthStencilTexture;
            Handle      m_normalTexture;
            Handle      m_colorTexture;
            Handle      m_linearDepthTexture;
            Handle      m_lightTexture;

            Handle      m_geomFramebuffer;
            Handle      m_linearizeDepthFramebuffer;
            Handle      m_lightFramebuffer;
        };

    public:
        void Initialize( DebugRenderer * debugRenderer );
        void Destroy();

        void Render( const Context& context );
        void Draw( RenderCache& renderCache );

        static void InitializeContext( Context& context, SizeT size, Camera * camera, Scene * scene );
        static void DestroyContext( Context& context );

    private:
        RenderList      m_opaqueList[6];

        MeshRenderer *  m_meshRenderer;
        LightRenderer   m_lightRenderer;
        DebugRenderer   m_debugRenderer;

        ProgramHandle   m_programLinearDepth;
        RenderState     m_renderStateLinearDepth;

        Handle          m_frameUniformBuffer[6];
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::EnvMapRenderer::Context );

#endif // _GRAPHIC_ENVMAPRENDERER_H

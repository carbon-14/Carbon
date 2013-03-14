#pragma once
#ifndef _GRAPHIC_ENVMAPRENDERER_H
#define _GRAPHIC_ENVMAPRENDERER_H

#include "Graphic/RenderList.h"
#include "Graphic/Camera.h"
#include "Graphic/DebugRenderer.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"

namespace Graphic
{
    class Scene;

    class _GraphicExport EnvMapRenderer
    {
    public:
        struct Context
        {
            Camera *                    m_camera;
            Scene *                     m_scene;

            SizeT                       m_size;
            Handle                      m_depthStencilTexture;
            Handle                      m_normalTexture;
            Handle                      m_colorTexture;
            Handle                      m_linearDepthTexture;

            Handle                      m_lightTexture;

            Handle                      m_geomFramebuffer;
            Handle                      m_linearizeDepthFramebuffer;
            Handle                      m_lightFramebuffer;

            struct Face
            {
                Camera                  m_camera;
                Handle                  m_uniformBuffer;
                RenderList              m_opaqueList;

                DebugRenderer::Context *m_debugRendererContext;
                MeshRenderer::Context * m_meshRendererContext;
                LightRenderer::Context *m_lightRendererContext;
            }                           m_cube[6];
        };

    public:
        void Initialize( DebugRenderer * debugRenderer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer );
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT size, Camera * camera, Scene * scene );
        static void DestroyContext( Context * context );

        void Render( Context * context ) const;
        void Draw( Context * context, RenderCache& renderCache ) const;

    private:
        void LinearizeDepth( Context * context, RenderCache& renderCache ) const;

        DebugRenderer * m_debugRenderer;
        MeshRenderer *  m_meshRenderer;
        LightRenderer * m_lightRenderer;

        ProgramHandle   m_programLinearDepth;
        RenderState     m_renderStateLinearDepth;
    };
}

#endif // _GRAPHIC_ENVMAPRENDERER_H

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
            SizeT                       m_tileSize;
            Handle                      m_depthStencilTexture;
            Handle                      m_normalTexture;
            Handle                      m_colorTexture;
            Handle                      m_depthTexture;
            Handle                      m_stencilTexture;
            Handle                      m_tiledDepthBuffer;

            Handle                      m_lightTexture;
            Handle                      m_lightBlurTexture;

            Handle                      m_geomFramebuffer;
            Handle                      m_resolveDepthFramebuffer;
            Handle                      m_resolveStencilFramebuffer;
            Handle                      m_lightFramebuffer;

            struct Face
            {
                Camera                  m_camera;
                Handle                  m_uniformBuffer;
                RenderList              m_opaqueList;

                DebugRenderer::Context *m_debugRendererContext;
                Rasterizer::Context *   m_rasterizerContext;
                MeshRenderer::Context * m_meshRendererContext;
                LightRenderer::Context *m_lightRendererContext;
            }                           m_cube[6];
        };

    public:
        void Initialize( DebugRenderer * debugRenderer, Rasterizer * rasterizer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer );
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT size, Camera * camera, Scene * scene );
        static void DestroyContext( Context * context );

        void Render( Context * context ) const;
        void Draw( const Context * context, RenderCache& renderCache ) const;

    private:
        void ResolveDepth( const Context * context, RenderCache& renderCache ) const;
        void ResolveStencil( const Context * context, RenderCache& renderCache ) const;
        void TileDepth( const Context * context, RenderCache& renderCache ) const;

        DebugRenderer * m_debugRenderer;
        Rasterizer *    m_rasterizer;
        MeshRenderer *  m_meshRenderer;
        LightRenderer * m_lightRenderer;

        ProgramHandle   m_programResolveDepth;
        ProgramHandle   m_programResolveStencil;
        ProgramHandle   m_programTileDepth;
        ProgramHandle   m_programBlur[3];

        RenderState     m_renderStateResolveDepthStencil;

        Handle          m_blurUniformBuffer;
    };
}

#endif // _GRAPHIC_ENVMAPRENDERER_H

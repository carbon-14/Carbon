#pragma once
#ifndef _GRAPHIC_FRAMERENDERER_H
#define _GRAPHIC_FRAMERENDERER_H

#include "Graphic/RenderList.h"
#include "Graphic/DebugRenderer.h"
#include "Graphic/Rasterizer.h"
#include "Graphic/MeshRenderer.h"
#include "Graphic/LightRenderer.h"
#include "Graphic/EnvMapRenderer.h"

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
        Vector  m_cameraPosition;
        Matrix  m_viewMatrix;
        Matrix  m_projMatrix;
        Matrix  m_viewProjMatrix;
    };

    class _GraphicExport FrameRenderer
    {
    public:
        struct Context
        {
            Camera *                    m_camera;
            Scene *                     m_scene;

            SizeT                       m_width;
            SizeT                       m_height;
            SizeT                       m_tileWidth;
            SizeT                       m_tileHeight;
            Handle                      m_depthStencilTexture;
            Handle                      m_normalTexture;
            Handle                      m_colorTexture;
            Handle                      m_depthTexture;
            Handle                      m_stencilTexture;
            Handle                      m_tiledDepthBuffer;

            Handle                      m_lightTexture;
            Handle                      m_finalColorBuffer;
            
            Handle                      m_geomFramebuffer;
            Handle                      m_resolveDepthFramebuffer;
            Handle                      m_resolveStencilFramebuffer;
            Handle                      m_lightFramebuffer;
            Handle                      m_finalFramebuffer;

            Handle                      m_uniformBuffer;
            RenderList                  m_opaqueList;

            DebugRenderer::Context *    m_debugRendererContext;
            Rasterizer::Context *		m_rasterizerContext;
            MeshRenderer::Context *     m_meshRendererContext;
            LightRenderer::Context *    m_lightRendererContext;
            EnvMapRenderer::Context *   m_envMapRendererContext;
        };

    public:
        FrameRenderer();
        ~FrameRenderer();

        void Initialize( DebugRenderer * debugRenderer, Rasterizer * rasterizer, MeshRenderer * meshRenderer, LightRenderer * lightRenderer, EnvMapRenderer * envMapRenderer );
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context, SizeT width, SizeT height, Camera * camera, Scene * scene, SizeT envMapSize );
        static void DestroyContext( Context * context );

        void Render( Context * context ) const;
        void Draw( const Context * context, RenderCache& renderCache ) const;

    private:
        void ResolveDepth( const Context * context, RenderCache& renderCache ) const;
        void ResolveStencil( const Context * context, RenderCache& renderCache ) const;
        void TileDepth( const Context * context, RenderCache& renderCache ) const;
        void ApplyToneMapping( const Context * context, RenderCache& renderCache ) const;
        void DrawOverlay( const Context * context, RenderCache& renderCache ) const;

        DebugRenderer *     m_debugRenderer;
        Rasterizer *		m_rasterizer;
        MeshRenderer *      m_meshRenderer;
        LightRenderer *     m_lightRenderer;
        EnvMapRenderer *    m_envMapRenderer;

        ProgramHandle       m_programResolveDepth;
        ProgramHandle       m_programResolveStencil;
        ProgramHandle       m_programTileDepth;
        ProgramHandle       m_programToneMapping;
        ProgramHandle       m_programOverlay;

        RenderState         m_renderStateResolveDepthStencil;
        RenderState         m_renderStateToneMapping;
        RenderState         m_renderStateOverlay;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::FrameParameters );

#endif // _GRAPHIC_FRAMERENDERER_H

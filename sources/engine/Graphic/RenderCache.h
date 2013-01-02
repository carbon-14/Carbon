#pragma once
#ifndef _GRAPHIC_RENDERCACHE_H
#define _GRAPHIC_RENDERCACHE_H

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderState.h"

namespace Graphic
{
    class _GraphicExport RenderCache
    {
    public:
        RenderCache( const ProgramCache& programCache );
        ~RenderCache();

        void Clear();

        void SetClearColor( F32 r, F32 g, F32 b, F32 a );
        void SetClearDepth( F32 depth );
        void SetClearStencil( U8 stencil );
        void SetSRGBWrite( Bool enable );
        void SetProgram( ProgramHandle program );
        void SetRenderState( const RenderState& renderState );
        void SetTextureUnits( const TextureUnit * textureUnits, SizeT count );
        void SetUniformBuffers( const Handle * uniformBuffers, SizeT count );

    private:
        const ProgramCache& m_programCache;

        F32                 m_clearColor[4];
        F32                 m_clearDepth;
        U8                  m_clearStencil;
        Bool                m_sRGBWrite;
        ProgramHandle       m_program;
        RenderState         m_renderState;
        TextureUnit         m_textureUnits[ RenderDevice::ms_maxTextureUnitCount ];
        SizeT               m_textureUnitCount;
        Handle              m_uniformBuffers[ RenderDevice::ms_maxUniformBufferCount ];
        SizeT               m_uniformBufferCount;
    };
}

#endif // _GRAPHIC_RENDERCACHE_H

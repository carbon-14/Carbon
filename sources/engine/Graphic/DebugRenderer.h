#pragma once
#ifndef _GRAPHIC_DEBUGRENDERER_H
#define _GRAPHIC_DEBUGRENDERER_H

#include "Graphic/RenderState.h"
#include "Graphic/ProgramCache.h"

#include "Core/Vector.h"
#include "Core/Array.h"

namespace Graphic
{
    class RenderCache;

    class _GraphicExport DebugRenderer
    {
    public:
        void Initialize();
        void Destroy();

        void RenderLine( const Vector& position0, const Vector& position1, const Vector& color );
        void Draw( RenderCache& renderCache, Handle frameParameters );

    private:
        struct Vertex
        {
            F32 m_pos[3];
            U8 m_color[4];
        };

        static VertexDeclaration CreateVertexDeclaration();
        static const VertexDeclaration  m_vdecl;

        Array< Vertex > m_vertices;
        ProgramHandle   m_program;
        RenderState     m_state;
        Handle          m_vertexBuffer;
        SizeT           m_vertexBufferSize;
        Handle          m_vertexArray;
    };
}

#endif // _GRAPHIC_DEBUGRENDERER_H

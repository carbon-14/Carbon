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

    struct _GraphicExport DebugLine
    {
        Vector  m_position0;
        Vector  m_position1;
        Vector  m_color;
    };

    class _GraphicExport DebugRenderer
    {
    public:
        struct Vertex
        {
            F32 m_pos[3];
            U8 m_color[4];
        };

        struct Context
        {
            Array< Vertex > m_vertices;
            Handle          m_vertexBuffer;
            SizeT           m_vertexBufferSize;
            Handle          m_vertexArray;
        };

    public:
        void Initialize();
        void Destroy();

        static Context * CreateContext();
        static void UpdateContext( Context * context );
        static void DestroyContext( Context * context );

        void Render( const DebugLine& line, Context * context ) const;
        void Draw( const Context * context, RenderCache& renderCache ) const;

    private:
        static VertexDeclaration CreateVertexDeclaration();
        static const VertexDeclaration  m_vdecl;

        ProgramHandle   m_program;
        RenderState     m_state;
    };
}

#endif // _GRAPHIC_DEBUGRENDERER_H

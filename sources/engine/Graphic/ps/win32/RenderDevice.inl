#include "Graphic/DLL.h"

#include <Windows.h>

#include "Graphic/OpenGL.h"

namespace Graphic
{
    class _GraphicExport RenderDevice
    {
    public:
        RenderDevice();
        ~RenderDevice();

        Bool                    Initialize( HWND window );
        void                    Destroy();
        void                    Swap();

        static VertexArray *    CreateVertexArray( const AttribDeclaration attribDecl[], SizeT attribCount, SizeT vertexSize, SizeT vertexCount, const void * vertexData, DataType indexType, SizeT indexCount, const void * indexData, VertexArrayUsage usage );
        static void             DestroyVertexArray( VertexArray * va );

        static Handle           CreateProgram( const Char * srcBuffers[], SizeT srcSizes[], ShaderType srcTypes[], SizeT count );
        static void             DeleteProgram( Handle program );
        static void             GetProgramBinary( Handle program, void *& binary, SizeT& size );
        static Handle           CreateProgramBinary( const void * binary, SizeT size );
        static void             UseProgram( Handle program );

        static void             Draw( PrimitiveType primitive, VertexArray * va );

        static void             SetViewport( U32 x, U32 y, U32 w, U32 h );
        static void             ClearColor( F32 r, F32 g, F32 b, F32 a );

    private:
        HWND                    m_window;
        HDC                     m_deviceContext;
        HGLRC                   m_renderContext;
    };
}

#include "Graphic/DLL.h"
#include "Core/Types.h"

#include <Windows.h>

#include "Graphic/OpenGL.h"

namespace Graphic
{
    enum ShaderType
    {
        ST_VERTEX_SHADER = 0,
        ST_FRAGMENT_SHADER,
        ST_GEOMETRY_SHADER,
        ST_COUNT
    };

    enum DataType
    {
        DT_S8 = 0,
        DT_U8,
        DT_S16,
        DT_U16,
        DT_S32,
        DT_U32,
        DT_F32,
        DT_F64
    };

    const SizeT DataTypeSize[] = { 1, 1, 2, 2, 4, 4, 4, 8 };

    enum VertexSemantic
    {
        VS_POSITION = 0,
        VS_NORMAL,
        VS_TANGENT,
        VS_BINORMAL,
        VS_COLOR,
        VS_TEXCOORD0,
        VS_TEXCOORD1,
        VS_COUNT
    };

    enum VertexArrayUsage
    {
        VAU_STATIC = 0,
        VAU_DYNAMIC,
        VAU_STREAM
    };

    struct AttribDeclaration
    {
        VertexSemantic  m_semantic;
        DataType        m_type;
        SizeT           m_size;
        Bool            m_normalized;
        SizeT           m_offset;
    };

    struct VertexArray;

    class _GraphicExport RenderDevice
    {
    public:
        RenderDevice();
        ~RenderDevice();

        Bool        Initialize( HWND window );
        void        Destroy();
        void        Swap();

        static VertexArray * CreateVertexArray( const AttribDeclaration attribDecl[], SizeT attribCount, SizeT vertexSize, SizeT vertexCount, const void * vertexData, DataType indexType, SizeT indexCount, const void * indexData, VertexArrayUsage usage );
        static void DestroyVertexArray( VertexArray * va );

        static U32  CreateProgram( const Char * srcBuffers[], SizeT srcSizes[], ShaderType srcTypes[], SizeT count );
        static void DeleteProgram( U32 handle );
        static void GetProgramBinary( U32 handle, void *& binary, SizeT& size );
        static U32  CreateProgramBinary( const void * binary, SizeT size );

        static void SetViewport( U32 x, U32 y, U32 w, U32 h );
        static void ClearColor( F32 r, F32 g, F32 b, F32 a );

    private:
        HWND        m_window;
        HDC         m_deviceContext;
        HGLRC       m_renderContext;
    };
}

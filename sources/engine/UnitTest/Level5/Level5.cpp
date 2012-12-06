#include "UnitTest/Level5/Level5.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderList.h"

#include "Core/TimeUtils.h"

using namespace Core;
using namespace Graphic;

namespace Level5_NS
{
    const U32 FRAME_MAX_COUNT = 60;
    U64 clockTicks = 0;
    U32 frameCount = 0;

    const SizeT frameAllocatorSize = 10 * 1024;

    const U32 frameWidth    = 1280;
    const U32 frameHeight   = 720;
    const F32 frameRatio    = (F32)frameWidth/(F32)frameHeight;

    RenderDevice device3d;
    ProgramCache programCache;
    RenderList renderList;

    enum VType
    {
        VT_UBYTE    = 0,
        VT_SHORT,
        VT_HALF,
        VT_FLOAT,
        VT_NMVECT      // 2_10_10_10
    };

    const DataType toAttribType[] =
    {
        DT_U8,
        DT_S16,
        DT_F16,
        DT_F32,
        DT_S2_10_10_10
    };

    const SizeT toAttribSize[] =
    {
        1, 
        2, 
        2,
        4,
        1   // used to keep a size of 4 in vertex declaration
    };

    const Bool toAttribNormalized[] =
    {
        false,
        false,
        false,
        false,
        true
    };

    enum IType
    {
        IT_UBYTE   = 0,
        IT_USHORT,
        IT_ULONG
    };

    const DataType toIndexType[] =
    {
        DT_U8,
        DT_U16,
        DT_U32
    };

    const SizeT toIndexSize[] =
    {
        1, 
        2, 
        4
    };

    struct MeshHeader
    {
        IType   indexType;
        U32     subMeshCount;
        U32     inputCount;
        U32     vertexDataSize;
    };

    struct MeshInputDesc
    {
        VertexSemantic  semantic;
        VType           type;
        U32             size;
        U32             offset;
    };

    Geometry LoadMesh( const Char * filename )
    {
        Geometry geom;

        PathString path;
        FileSystem::BuildPathName( filename, path, FileSystem::PT_CACHE );

        void * data;
        SizeT size;
        FileSystem::Load( path, data, size );

        U8 * ptr = (U8*)data;

        MeshHeader * header = (MeshHeader*)ptr;
        ptr += sizeof(MeshHeader);

        geom.m_indexType    = toIndexType[ header->indexType ];
        geom.m_subGeomCount = header->subMeshCount;

        VertexDeclaration& vDecl = geom.m_vertexDecl;
        vDecl.m_count = header->inputCount;
        vDecl.m_size = 0;
        for ( SizeT i=0; i<vDecl.m_count; ++i )
        {
            MeshInputDesc * desc = (MeshInputDesc*)ptr;
            ptr += sizeof(MeshInputDesc);

            vDecl.m_attributes[ i ].m_semantic      = desc->semantic;
            vDecl.m_attributes[ i ].m_type          = toAttribType[ desc->type ];
            vDecl.m_attributes[ i ].m_size          = desc->size / toAttribSize[ desc->type ];
            vDecl.m_attributes[ i ].m_normalized    = toAttribNormalized[ desc->type ];
            vDecl.m_attributes[ i ].m_offset        = desc->offset;
            vDecl.m_size                            += desc->size;
        }

        geom.m_vertexBuffer = RenderDevice::CreateVertexBuffer( header->vertexDataSize, ptr, BU_STATIC );

        ptr += header->vertexDataSize;

        for ( SizeT i=0; i<geom.m_subGeomCount; ++i )
        {
            SizeT count = *((U32*)ptr);
            ptr += sizeof(U32);

            SizeT size = count * DataTypeSize[ geom.m_indexType ];

            geom.m_subGeoms[ i ].m_indexCount   = count;
            geom.m_subGeoms[ i ].m_indexBuffer  = RenderDevice::CreateIndexBuffer( size, ptr, BU_STATIC );
            geom.m_subGeoms[ i ].m_vertexArray  = RenderDevice::CreateVertexArray( geom.m_vertexDecl, geom.m_vertexBuffer, geom.m_subGeoms[ i ].m_indexBuffer );
            ptr += size;
        }

        UnknownAllocator::Deallocate( data );

        return geom;
    }

    class FullScreenQuadRenderer
    {
    public:
        FullScreenQuadRenderer()
        {
        }

        void Initialize()
        {
            m_renderElement.m_primitive = PT_TRIANGLES;
            m_renderElement.m_program   = programCache.GetProgram( "level5" );

            m_renderElement.m_geom = LoadMesh( "sibenik.bmh" );

            m_renderElement.m_unitCount = 0;
        }

        void Render()
        {
            renderList.Push( m_renderElement );
        }

        void Destroy()
        {
            Geometry& geom = m_renderElement.m_geom;
            for ( SizeT i=0; i<geom.m_subGeomCount; ++i )
            {
                RenderDevice::DestroyVertexArray( geom.m_subGeoms[ i ].m_vertexArray );
                RenderDevice::DestroyBuffer( geom.m_subGeoms[ i ].m_indexBuffer );
            }
            RenderDevice::DestroyBuffer( geom.m_vertexBuffer );
        }

    private:
        RenderElement   m_renderElement;
    };

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch(msg)
        {
            case WM_CLOSE:
                DestroyWindow(hwnd);
            break;
            case WM_DESTROY:
                PostQuitMessage(0);
            break;
            case WM_KEYDOWN:
		    switch ( wParam )
		    {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
		        case VK_SPACE:
                {
                    programCache.NotifySourceChange();
                    break;
                }
		    }
		    return 0;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }

    Bool MessagePump( MSG * msg )
    {
        // check for messages
        while ( PeekMessage( msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // handle or dispatch messages
            if ( msg->message == WM_QUIT )
            {
                return false;
            }
            else
            {
                TranslateMessage( msg );
                DispatchMessage( msg );
            }
        }

        return true;
    }

    void DisplayFramerate( HWND hwnd )
    {
        ++frameCount;
        if ( frameCount == FRAME_MAX_COUNT )
        {
            U64 currentTicks    = Core::TimeUtils::ClockTime();
            F64 fps             = FRAME_MAX_COUNT * Core::TimeUtils::ClockFrequency() / ( currentTicks - clockTicks );
            clockTicks          = currentTicks;
            frameCount          = 0;

            Char text[ 32 ];
            SetWindowText( hwnd, Core::StringUtils::FormatString( text, 32, "Level4 - [ %0.0f fps ]", fps ) );
        }
    }
}

using namespace Level5_NS;

WPARAM Level5( HINSTANCE hInstance, int nCmdShow )
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 5 #\n###########\n\n" );
    UNIT_TEST_MESSAGE( "Window Creation\n" );

    MSG msg;
    HWND hwnd;
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "CarbonWndClass";

    if ( !RegisterClass(&wc) ) return FALSE;

    hwnd = CreateWindow( "CarbonWndClass"
                        ,"Level5"
                        ,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                        ,CW_USEDEFAULT
                        ,CW_USEDEFAULT
                        ,frameWidth
                        ,frameHeight
                        ,NULL
                        ,NULL
                        ,hInstance
                        ,NULL );

    if ( !hwnd ) return FALSE;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    UNIT_TEST_MESSAGE( "Carbon Engine : Initialize\n" );

    MemoryManager::Initialize( frameAllocatorSize );
    FileSystem::Initialize( "D:\\GitDepot\\Carbon\\" );

    if ( ! device3d.Initialize( hwnd ) )
    {
        return FALSE;
    }

    if ( ! programCache.Initialize( "shaders\\" ) )
    {
        device3d.Destroy();
        return FALSE;
    }

    renderList.SetSRGBWrite( true );

    FullScreenQuadRenderer fsqRenderer;
    fsqRenderer.Initialize();

    UNIT_TEST_MESSAGE( "Carbon Engine : Run\n" );

    while ( 1 )
    {
        if ( ! MessagePump( &msg ) ) break;

        /////////////////////////////////////////////////////////////////////////////

        programCache.Update();

        fsqRenderer.Render();

        renderList.Draw( programCache );

        device3d.Swap();

        MemoryManager::FrameUpdate();

        /////////////////////////////////////////////////////////////////////////////

        DisplayFramerate( hwnd );
    }

    UNIT_TEST_MESSAGE( "Carbon Engine : Destroy\n" );

    fsqRenderer.Destroy();

    programCache.Destroy();

    device3d.Destroy();

    FileSystem::Destroy();
    MemoryManager::Destroy();

    DestroyWindow( hwnd );
    UnregisterClass( "CarbonWndClass", hInstance );

    return msg.wParam;
}

#include "UnitTest/Level4/Level4.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderList.h"
#include "Graphic/RenderGeometry.h"

#include "Core/TimeUtils.h"

using namespace Core;
using namespace Graphic;

namespace Level4_NS
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

    struct TextureHeader
    {
        unsigned int    internalFormat;
        unsigned int    externalFormat;
        unsigned int    mipMapCount;
        bool            compressed;
    };

    struct LevelDesc
    {
        unsigned int    size;
        unsigned int    width;
        unsigned int    height;
    };

    Handle LoadTexture( const Char * filename )
    {
        PathString path;
        FileSystem::BuildPathName( filename, path, FileSystem::PT_CACHE );

        void * data;
        SizeT size;
        FileSystem::Load( path, data, size );

        U8 * ptr = (U8*)data;

        TextureHeader * header = (TextureHeader*)ptr;

        ptr += sizeof(TextureHeader);

        const SizeT maxMipMapCount = 16;
        SizeT sizes[ maxMipMapCount ];
        SizeT widths[ maxMipMapCount ];
        SizeT heights[ maxMipMapCount ];
        void * datas[ maxMipMapCount ];

        SizeT count = ( header->mipMapCount < maxMipMapCount ) ? header->mipMapCount : maxMipMapCount;
        for ( SizeT i=0; i<count; ++i )
        {
            LevelDesc * desc = (LevelDesc*)ptr;
            ptr += sizeof(LevelDesc);

            sizes[ i ]      = desc->size;
            widths[ i ]     = desc->width;
            heights[ i ]    = desc->height;
            datas[ i ]      = ptr;
            ptr += desc->size;
        }

        Handle texture = RenderDevice::CreateTexture( header->internalFormat, header->externalFormat, count, header->compressed, sizes, widths, heights, datas );

        UnknownAllocator::Deallocate( data );

        return texture;
    }

    class RenderSimple : public RenderGeometry
    {
    public:
        void Draw()
        {
            RenderDevice::BeginGeometry( m_vertexDecl, m_vertexArray, m_indexBuffer );
            RenderDevice::DrawIndexed( m_primitive, m_indexCount, m_indexType );
            RenderDevice::EndGeometry( m_vertexDecl );
        }

        PrimitiveType       m_primitive;
        Handle              m_vertexBuffer;
        VertexDeclaration   m_vertexDecl;
        SizeT               m_vertexCount;
        DataType            m_indexType;
        Handle              m_vertexArray;
        Handle              m_indexBuffer;
        SizeT               m_indexCount;
    };

    class FullScreenQuadRenderer
    {
    public:
        FullScreenQuadRenderer()
        {
        }

        void Initialize()
        {
            U32 programId   = ProgramCache::CreateId( "level4" );
            m_program       = programCache.GetProgram( programId );

            m_geom.m_primitive = PT_TRIANGLES;

            VertexDeclaration& vDecl            = m_geom.m_vertexDecl;
            vDecl.m_attributes[0].m_semantic    = VS_POSITION;
            vDecl.m_attributes[0].m_type        = DT_F32;
            vDecl.m_attributes[0].m_size        = 4;
            vDecl.m_attributes[0].m_normalized  = false;
            vDecl.m_attributes[0].m_offset      = 0;
            vDecl.m_size                        = 16;
            vDecl.m_count                       = 1;

            m_geom.m_vertexCount = 4;
            const F32 vb[4][4] =
            {
                { -1.0f, -1.0f, -frameRatio, -1.0f },
                { +1.0f, -1.0f, +frameRatio, -1.0f },
                { -1.0f, +1.0f, -frameRatio, +1.0f },
                { +1.0f, +1.0f, +frameRatio, +1.0f }
            };

            m_geom.m_vertexBuffer = RenderDevice::CreateVertexBuffer( sizeof(vb), vb, BU_STATIC );

            m_geom.m_indexType = DT_U8;

            const U8 ib[2][3] =
            {
                0, 1, 2,
                2, 1, 3,
            };

            m_geom.m_indexBuffer = RenderDevice::CreateIndexBuffer( sizeof(ib), ib, BU_STATIC );
            m_geom.m_vertexArray = RenderDevice::CreateVertexArray( vDecl, m_geom.m_vertexBuffer, m_geom.m_indexBuffer );
            m_geom.m_indexCount  = 6;

            m_textures[0] = LoadTexture( "default_c.btx" );
            m_textures[1] = LoadTexture( "default_n.btx" );
        }

        void Render()
        {
            RenderElement element;

            element.m_program = m_program;

            for ( element.m_textureCount = 0; element.m_textureCount<2; ++element.m_textureCount )
            {
                element.m_textures[ element.m_textureCount ].m_handle   = m_textures[ element.m_textureCount ];
                element.m_textures[ element.m_textureCount ].m_index    = element.m_textureCount;
            }

            element.m_uniformBufferCount    = 0;
            element.m_geometry              = &m_geom;

            renderList.Push( element );
        }

        void Destroy()
        {
            RenderDevice::DestroyTexture( m_textures[0] );
            RenderDevice::DestroyTexture( m_textures[1] );

            RenderDevice::DestroyVertexArray( m_geom.m_vertexArray );
            RenderDevice::DestroyBuffer( m_geom.m_indexBuffer );
            RenderDevice::DestroyBuffer( m_geom.m_vertexBuffer );
        }

    private:
        ProgramHandle   m_program;
        RenderSimple    m_geom;
        Handle          m_textures[2];
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

using namespace Level4_NS;

WPARAM Level4( HINSTANCE hInstance, int nCmdShow )
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 4 #\n###########\n\n" );
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
                        ,"Level4"
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
    FileSystem::Initialize( "../../.." );

    if ( ! device3d.Initialize( hInstance, hwnd ) )
    {
        return FALSE;
    }

    if ( ! programCache.Initialize( "shaders" ) )
    {
        device3d.Destroy();
        return FALSE;
    }

    RenderCache renderCache;

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

        renderList.Draw( renderCache );

        renderCache.Clear();

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

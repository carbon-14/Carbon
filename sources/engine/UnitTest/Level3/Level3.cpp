#include "UnitTest/Level3/Level3.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"
#include "Graphic/RenderList.h"

#include "Core/TimeUtils.h"

using namespace Core;
using namespace Graphic;

const U32 FRAME_MAX_COUNT = 60;
U64 clockTicks = 0;
U32 frameCount = 0;

const SizeT frameAllocatorSize = 10 * 1024;

RenderDevice device3d;
ProgramCache programCache;
RenderList renderList;

class FullScreenQuadRenderer
{
public:
    FullScreenQuadRenderer()
    {
    }

    void Initialize()
    {
        m_renderElement.m_primitive = PT_TRIANGLES;
        m_renderElement.m_program   = programCache.GetProgram( "fullScreenQuad" );

        const AttribDeclaration vDecl[] =
        {
            { VS_POSITION, DT_F32, 4, false, 0 }
        };
        const F32 vb[4][4] =
        {
            { -1.0f, -1.0f, 0.0f, 1.0f },
            { +1.0f, -1.0f, 1.0f, 1.0f },
            { -1.0f, +1.0f, 0.0f, 0.0f },
            { +1.0f, +1.0f, 1.0f, 0.0f }
        };
        const U8 ib[2][3] =
        {
            0, 1, 2,
            2, 1, 3,
        };

        m_renderElement.m_vertexArray = RenderDevice::CreateVertexArray( vDecl, 1, 16, 4, (const void*)vb, DT_U8, 6, (const void*)ib, VAU_STATIC );
    }

    void Render()
    {
        renderList.Push( m_renderElement );
    }

    void Destroy()
    {
        RenderDevice::DestroyVertexArray( m_renderElement.m_vertexArray );
    }

private:
    RenderElement m_renderElement;
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
        SetWindowText( hwnd, Core::StringUtils::FormatString( text, 32, "Level3 - [ %0.0f fps ]", fps ) );
    }
}

WPARAM Level3( HINSTANCE hInstance, int nCmdShow )
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 3 #\n###########\n\n" );
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
                        ,"Level3"
                        ,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                        ,CW_USEDEFAULT
                        ,CW_USEDEFAULT
                        ,1280
                        ,720
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

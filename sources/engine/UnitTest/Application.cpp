#include "UnitTest/Application.h"
#include "UnitTest/Utils.h"

#include "Core/FileSystem.h"
#include "Core/ResourceManager.h"

#include "Core/TimeUtils.h"

using namespace Graphic;

const SizeT frameAllocatorSize  = 10 * 1024;

const SizeT frameWidth          = 1280;
const SizeT frameHeight         = 720;

const U32 FRAME_MAX_COUNT       = 60;

Bool RegisterInputDevices( HWND hwnd )
{
    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage  = 0x01;
    Rid[0].usUsage      = 0x02;
    Rid[0].dwFlags      = RIDEV_INPUTSINK;
    Rid[0].hwndTarget   = hwnd;

    Rid[1].usUsagePage  = 0x01;
    Rid[1].usUsage      = 0x06;
    Rid[1].dwFlags      = RIDEV_INPUTSINK;
    Rid[1].hwndTarget   = hwnd;

    return ( RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == TRUE );
}

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
		    if ( wParam == VK_ESCAPE )
		        PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

BOOL CreateRenderWindow( HINSTANCE hInstance, RenderWindow& window )
{
    window.width = frameWidth;
    window.height = frameHeight;
    window.hInstance = hInstance;

    window.wc.style         = 0;
    window.wc.lpfnWndProc   = WndProc;
    window.wc.cbClsExtra    = 0;
    window.wc.cbWndExtra    = 0;
    window.wc.hInstance     = hInstance;
    window.wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    window.wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window.wc.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
    window.wc.lpszMenuName  =  NULL;
    window.wc.lpszClassName = "CarbonWndClass";

    if ( !RegisterClass(&window.wc) ) return FALSE;

    window.hwnd = CreateWindow( "CarbonWndClass",
                                "CarbonEngine",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                frameWidth,
                                frameHeight,
                                NULL,
                                NULL,
                                hInstance,
                                NULL );

    if ( !window.hwnd )
    {
        MessageBox( window.hwnd, "Cannot create the window !", "Fatal Error", MB_OK );
        return FALSE;
    }

    RECT rcClient, rcWind;
    POINT ptDiff;
    GetClientRect(window.hwnd, &rcClient);
    GetWindowRect(window.hwnd, &rcWind);
    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
    MoveWindow(window.hwnd,rcWind.left, rcWind.top, frameWidth + ptDiff.x, frameHeight + ptDiff.y, TRUE);

    return TRUE;
}

void DestroyRenderWindow( const RenderWindow& window )
{
    DestroyWindow( window.hwnd );
    UnregisterClass( "CarbonWndClass", window.hInstance );
}

Application::Application( const RenderWindow& window )
    : m_window( window )
    , m_clockTicks( 0 )
    , m_frameTicks( 0 )
    , m_ticksCount( 0 )
    , m_frameCount( 0 )
{
}

WPARAM Application::Run()
{
    UNIT_TEST_MESSAGE( "Carbon Engine : Initialize\n" );
    if ( !Initialize() )
        return FALSE;

    UNIT_TEST_MESSAGE( "Carbon Engine : Run\n" );
    while( Update() );

    UNIT_TEST_MESSAGE( "Carbon Engine : Destroy\n" );
    Destroy();

    return TRUE;
}

Bool Application::Initialize()
{
    if ( ! RegisterInputDevices( m_window.hwnd ) )
    {
        MessageBox( m_window.hwnd, "Cannot initialize input devices !", "Fatal Error", MB_OK );
        return false;
    }

    MemoryManager::Initialize( frameAllocatorSize );
    FileSystem::Initialize( "../../.." );

    if ( ! m_renderDevice.Initialize( m_window.hInstance, m_window.hwnd ) )
    {
        MessageBox( m_window.hwnd, "Cannot initialize the 3D device !", "Fatal Error", MB_OK );
        return false;
    }

    if ( ! ProgramCache::Initialize( "shaders" ) )
    {
        m_renderDevice.Destroy();
        MessageBox( m_window.hwnd, "Cannot initialize the program cache !", "Fatal Error", MB_OK );
        return false;
    }

    ResourceManager::Initialize();

    PreExecute();

    return true;
}

void Application::Destroy()
{
    PostExecute();

    ResourceManager::Destroy();

    ProgramCache::Destroy();

    m_renderDevice.Destroy();

    FileSystem::Destroy();
    MemoryManager::Destroy();
}

Bool Application::Update()
{
    /////////////////////////////////////////////////////////////////////////////

    ProgramCache::Update();

    Execute();

    m_renderDevice.Swap();

    ResourceManager::Update();
    MemoryManager::FrameUpdate();

    /////////////////////////////////////////////////////////////////////////////

    U64 currentTicks    = TimeUtils::ClockTime();
    m_frameTicks        = currentTicks - m_clockTicks;
    m_clockTicks        = currentTicks;
    
    DisplayFramerate();

    return MessagePump( &m_window.msg );
}

Bool Application::MessagePump( MSG * msg )
{
    // check for messages
    while ( PeekMessage( msg, NULL, 0, 0, PM_REMOVE ) )
    {
        // handle or dispatch messages
        if ( msg->message == WM_QUIT )
            return false;
        
        if ( msg->message == WM_INPUT )
        {
            UINT dwSize;

            GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];

            if ( lpb ) 
            {
                if ( GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
                    OutputDebugString (TEXT("GetRawInputData does not return correct size !\n")); 

                ProcessInputs( (RAWINPUT*)lpb );

                delete[] lpb;
            }
        }
        else
        {
            TranslateMessage( msg );
            DispatchMessage( msg );
        }
    }

    return true;
}

void Application::DisplayFramerate()
{
    ++m_frameCount;
    m_ticksCount += m_frameTicks;

    if ( m_frameCount == FRAME_MAX_COUNT )
    {
        F64 fps = FRAME_MAX_COUNT * TimeUtils::ClockFrequency() / ( m_ticksCount );

        Char text[ 32 ];
        SetWindowText( m_window.hwnd, StringUtils::FormatString( text, 32, "CarbonEngine [ %0.0f fps ]", fps ) );

        m_frameCount = 0;
        m_ticksCount = 0;
    }
}

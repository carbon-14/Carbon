#include <Windows.h>

#include "UnitTest/Level0/Level0.h"
#include "UnitTest/Level1/Level1.h"
#include "UnitTest/Level2/Level2.h"
#include "UnitTest/Level3/Level3.h"
#include "UnitTest/Level4/Level4.h"
#include "UnitTest/Level5/Level5.h"
#include "UnitTest/Level6/Level6.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    //Level0();
    //Level1();
    //Level2();

    //return Level3( hInstance, nCmdShow );
    //return Level4( hInstance, nCmdShow );
    //return Level5( hInstance, nCmdShow );

    RenderWindow window;
    if ( ! CreateRenderWindow( hInstance, window ) )
    {
        return FALSE;
    }

    ShowWindow(window.hwnd, nCmdShow);
    UpdateWindow(window.hwnd);

    Level6 test( window );

    int r = test.Run();

    DestroyRenderWindow( window );

    return r;
}


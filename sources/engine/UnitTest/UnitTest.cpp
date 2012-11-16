#include <Windows.h>

#include "UnitTest/Level0/Level0.h"
#include "UnitTest/Level1/Level1.h"
#include "UnitTest/Level2/Level2.h"
#include "UnitTest/Level3/Level3.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    //Level0();
    //Level1();
    //Level2();

    return Level3( hInstance, nCmdShow );
}


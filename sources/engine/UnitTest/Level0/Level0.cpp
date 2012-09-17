#include "Level0.h"

#include <cstdio>

#include "Core/Platform.h"

void Level0()
{
    printf( "Hello World !!!" );
    printf( "Your platform is : %s\n", GetPlatformName() );
}

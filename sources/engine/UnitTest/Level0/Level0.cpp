#include "Level0.h"

#include "Core/Trace.h"
#include "Core/StringUtils.h"

#include "Core/Platform.h"

void Level0()
{
    Char buffer[64];
    CARBON_FORMAT_STRING( buffer, sizeof(buffer), "Your platform is : %s\n", GetPlatformName() );

    CARBON_TRACE( "Hello World !!!\n" );
    CARBON_TRACE( buffer );
}

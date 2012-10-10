#include "Level0.h"

#include "Core/Trace.h"
#include "Core/StringUtils.h"

#include "Core/Platform.h"

using namespace Core;

void Level0()
{
    Char buffer[64];
    StringUtils::FormatString( buffer, sizeof(buffer), "Your platform is : %s\n", Platform::GetName() );

    CARBON_TRACE( "Hello World !!!\n" );
    CARBON_TRACE( buffer );
}

#include "Core/Assert.h"

#include <cstdio>
#include <Windows.h>

void __CarbonAssertFail( const char * predicate, const char * file, int line, const char * msg )
{
    char buffer[512];
    if ( msg != NULL )
        sprintf_s( buffer, sizeof(buffer), "%s(%i): CARBON_ASSERT( %s, \"%s\" )\n\0", file, line, predicate, msg );
    else
        sprintf_s( buffer, sizeof(buffer), "%s(%i): CARBON_ASSERT( %s )\n\0", file, line, predicate );

    printf( buffer );
    OutputDebugStringA( buffer );
}

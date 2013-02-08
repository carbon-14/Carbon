#include "Core/Trace.h"

#include <cstdio>
#include <Windows.h>

void Trace::Message( const Char * msg )
{
    printf( "%s", msg );
    OutputDebugStringA( msg );
}

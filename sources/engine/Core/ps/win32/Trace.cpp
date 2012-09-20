#include "Trace.h"

#include <cstdio>
#include <Windows.h>

namespace Core
{
    void Trace::Message( const Char * msg )
    {
        printf( "%s", msg );
        OutputDebugStringA( msg );
    }
}


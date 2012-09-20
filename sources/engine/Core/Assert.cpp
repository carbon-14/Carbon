#include "Core/Assert.h"

#include "Core/Trace.h"
#include "Core/StringUtils.h"

void __CarbonAssertFail( const char * predicate, const char * file, int line, const char * msg )
{
    char buffer[512];
    if ( msg != 0 )
        CARBON_FORMAT_STRING( buffer, sizeof(buffer), "%s(%i): CARBON_ASSERT( %s, \"%s\" )\n\0", file, line, predicate, msg );
    else
        CARBON_FORMAT_STRING( buffer, sizeof(buffer), "%s(%i): CARBON_ASSERT( %s )\n\0", file, line, predicate );

    CARBON_TRACE( buffer );
}

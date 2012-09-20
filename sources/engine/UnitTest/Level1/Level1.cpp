#include "Level1.h"

#include <cstdlib>

#include "Core\Timer.h"

#define ALLOC_SIZE  16
#define ALLOC_COUNT 20000000

void * allocs[ ALLOC_COUNT ];

void Level1()
{
    {
        CARBON_AUTO_TIMER( allocID, "Plein d'allocs" );

        for ( int i=0; i<ALLOC_COUNT; ++i )
        {
            allocs[ i ] = (U8*)malloc( ALLOC_SIZE );
        }
    }

    for ( int i=0; i<100; ++i )
    {
        free( allocs[ i ] );
    }

    U8 * buffer;
    {
        CARBON_AUTO_TIMER( allocID, "1 grosse alloc" );

        buffer = (U8*)malloc( ALLOC_SIZE * ALLOC_COUNT );
        int offset = 0;
        for ( int i=0; i<ALLOC_COUNT; ++i, offset += ALLOC_SIZE )
        {
            allocs[ i ] = buffer + offset;

        }
    }

    free( buffer );
}

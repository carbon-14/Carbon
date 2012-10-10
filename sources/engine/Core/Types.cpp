#include "Core/Types.h"

#include "Core/Assert.h"

void __CheckTypeSizes()
{
    CARBON_COMPILE_TIME_ASSERT( sizeof( S8 ) == 1 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( S16 ) == 2 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( S32 ) == 4 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( S64 ) == 8 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( U8 ) == 1 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( U16 ) == 2 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( U32 ) == 4 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( U64 ) == 8 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( F32 ) == 4 );
    CARBON_COMPILE_TIME_ASSERT( sizeof( F64 ) == 8 );
}

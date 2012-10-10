#include "Core/Intrinsics.h"

#include "Core/Assert.h"

void __CheckIntrinsicsSizes()
{
    CARBON_COMPILE_TIME_ASSERT( sizeof( M128 ) == 16 );
}

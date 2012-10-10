#include "Core/MemoryUtils.h"

#include <cstring>

#include "Core/Assert.h"

namespace Core
{
    SizeT MemoryUtils::GetNextAlignedAddress( SizeT ptr, SizeT alignment )
    {
        CARBON_ASSERT( alignment > 0 );
        CARBON_ASSERT( ( alignment & (alignment-1) ) == 0 ); // alignment is power of 2

        return (ptr + (alignment - 1)) & ~(alignment - 1);
    }

    void * MemoryUtils::MemCpy( void * dest, const void * src, SizeT sizeBytes )
    {
        return memcpy( dest, src, sizeBytes );
    }

    void * MemoryUtils::MemSet( void * ptr, U8 value, SizeT sizeBytes )
    {
        return memset( ptr, value, sizeBytes );
    }
}

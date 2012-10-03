#pragma once
#ifndef _CORE_MEMORYUTILS_H
#define _CORE_MEMORYUTILS_H

#include "Core/Types.h"
#include "Core/DLL.h"

#include "Core/Assert.h"

class _CoreExport MemoryUtils
{
public:

    template< typename T >
    static SizeT AlignOf()
    {
#if defined( CARBON_PLATFORM_WIN32 )
        return __alignof( T );
#else
#error "alignof is not implemented."
#endif
    }

    static SizeT GetNextAlignedAddress( SizeT ptr, SizeT alignment )
    {
        CARBON_ASSERT( alignment > 0 );
        CARBON_ASSERT( ( alignment & (alignment-1) ) == 0 ); // alignment is power of 2

        return (ptr + (alignment - 1)) & ~(alignment - 1);
    }
};

#endif // _CORE_MEMORYUTILS_H

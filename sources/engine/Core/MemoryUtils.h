#pragma once
#ifndef _CORE_MEMORYUTILS_H
#define _CORE_MEMORYUTILS_H

#include "Core/Types.h"
#include "Core/DLL.h"

namespace Core
{
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

        static SizeT GetNextAlignedAddress( SizeT ptr, SizeT alignment );

        static void * MemCpy( void * dest, const void * src, SizeT sizeBytes );
        static void * MemSet( void * ptr, U8 value, SizeT sizeBytes );
    };
}

#endif // _CORE_MEMORYUTILS_H

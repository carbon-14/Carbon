#pragma once
#ifndef _CORE_MEMORYUTILS_H
#define _CORE_MEMORYUTILS_H

#include "Core/Types.h"
#include "Core/DLL.h"

class _CoreExport MemoryUtils
{
public:

    template< typename T >
    static SizeT AlignOf();

    static SizeT GetNextAlignedAddress( SizeT ptr, SizeT alignment );

    static void * MemCpy( void * dest, const void * src, SizeT sizeBytes );
    static void * MemSet( void * ptr, U8 value, SizeT sizeBytes );

    template < typename T >
    static void Fill( T * begin, const T * end, const T& value );

    template< typename T >
    static void Copy( const T * begin, const T * end, T * to );
};

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/MemoryUtils.inl"
#else
    #error Math not defined
#endif

#endif // _CORE_MEMORYUTILS_H

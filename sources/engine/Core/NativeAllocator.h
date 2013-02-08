#pragma once
#ifndef _CORE_NATIVEALLOCATOR_H
#define _CORE_NATIVEALLOCATOR_H

#include "Core/Types.h"
#include "Core/DLL.h"

class _CoreExport NativeAllocator
{
public:
    static void *   Malloc( SizeT sizeBytes, SizeT align = 1 );
    static void *   Realloc( void * ptr, SizeT sizeBytes, SizeT align = 1 );
    static void     Free( void * ptr );
};

#endif // _CORE_NATIVEALLOCATOR_H

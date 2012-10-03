#pragma once
#ifndef _CORE_DEFAULTALLOCATOR_H
#define _CORE_DEFAULTALLOCATOR_H

#include "Core/Types.h"
#include "Core/DLL.h"

namespace Core
{
    class _CoreExport DefaultAllocator
    {
    public:
        static void * Malloc( SizeT sizeBytes, SizeT align = 1 );
        static void * Realloc( void * ptr, SizeT sizeBytes, SizeT align = 1 );
        static void Free( void * ptr );
    };
}

#endif // _CORE_DEFAULTALLOCATOR_H

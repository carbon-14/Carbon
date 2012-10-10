#pragma once
#ifndef _CORE_MEMORYMANAGER_H
#define _CORE_MEMORYMANAGER_H

#include "Core/DLL.h"
#include "Core/Types.h"

namespace Core
{
    //====================================================================================
    // MemoryManager
    //====================================================================================

    class _CoreExport MemoryManager
    {
    public:
        static void Initialize( SizeT frameAllocatorSize );
        static void Finish();

        static void * Malloc( SizeT sizeBytes, SizeT align = 1 );
        static void * Realloc( void * ptr, SizeT sizeBytes, SizeT align = 1 );
        static void Free( void * ptr );

        static void FrameUpdate();
        static void * FrameAlloc( SizeT sizeBytes, SizeT align = 1 );
    };

    //====================================================================== MemoryManager

    //====================================================================================
    // DefaultAllocator
    //====================================================================================

    class _CoreExport DefaultAllocator
    {
    public:
        static void * Malloc( SizeT sizeBytes, SizeT align = 1 );
        static void * Realloc( void * ptr, SizeT sizeBytes, SizeT align = 1 );
        static void Free( void * ptr );
    };

    //=================================================================== DefaultAllocator
}

#endif // _CORE_MEMORYMANAGER_H

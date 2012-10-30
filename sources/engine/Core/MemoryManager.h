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

    class DefaultAllocator
    {
    public:
        static void *   Allocate( SizeT sizeBytes, SizeT align = 1 );
        static void     Deallocate( void * ptr );
    };

    //=================================================================== DefaultAllocator

    inline void * DefaultAllocator::Allocate( SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::Malloc( sizeBytes, align );
    }

    inline void DefaultAllocator::Deallocate( void * ptr )
    {
        MemoryManager::Free( ptr );
    }

    //=================================================================== DefaultAllocator

    //====================================================================================
    // FrameAllocator
    //====================================================================================

    class FrameAllocator
    {
    public:
        static void *   Allocate( SizeT sizeBytes, SizeT align = 1 );
        static void     Deallocate( void * ptr );
    };

    //===================================================================== FrameAllocator

    inline void * FrameAllocator::Allocate( SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::FrameAlloc( sizeBytes, align );
    }

    inline void FrameAllocator::Deallocate( void * )
    {
        // Do nothing
    }

    //===================================================================== FrameAllocator
}

#endif // _CORE_MEMORYMANAGER_H

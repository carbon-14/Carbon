#include "Core/MemoryManager.h"

#include "Core/NativeAllocator.h"
#include "Core/StackAllocator.h"

namespace Core
{
    //====================================================================================
    // MemoryManager
    //====================================================================================

    static StackAllocator frameAllocator;

    void MemoryManager::Initialize( SizeT frameAllocatorSize )
    {
        frameAllocator.Initialize( frameAllocatorSize );
    }

    void MemoryManager::Finish()
    {
        frameAllocator.Finish();
    }

    void * MemoryManager::Malloc( SizeT sizeBytes, SizeT align )
    {
        return NativeAllocator::Malloc( sizeBytes, align );
    }

    void * MemoryManager::Realloc( void * ptr, SizeT sizeBytes, SizeT align )
    {
        return NativeAllocator::Realloc( ptr, sizeBytes, align );
    }

    void MemoryManager::Free( void * ptr )
    {
        NativeAllocator::Free( ptr );
    }

    void MemoryManager::FrameUpdate()
    {
        frameAllocator.Clear();
    }

    void * MemoryManager::FrameAlloc( SizeT sizeBytes, SizeT align )
    {
        return frameAllocator.Allocate( sizeBytes, align );
    }

    //====================================================================== MemoryManager

    //====================================================================================
    // DefaultAllocator
    //====================================================================================

    void * DefaultAllocator::Malloc( SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::Malloc( sizeBytes, align );
    }

    void * DefaultAllocator::Realloc( void * ptr, SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::Realloc( ptr, sizeBytes, align );
    }

    void DefaultAllocator::Free( void * ptr )
    {
        MemoryManager::Free( ptr );
    }

    //=================================================================== DefaultAllocator
}

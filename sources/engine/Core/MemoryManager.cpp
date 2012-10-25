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

    void * DefaultAllocator::Allocate( SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::Malloc( sizeBytes, align );
    }

    void DefaultAllocator::Deallocate( void * ptr )
    {
        MemoryManager::Free( ptr );
    }

    //=================================================================== DefaultAllocator

    //====================================================================================
    // FrameAllocator
    //====================================================================================

    void * FrameAllocator::Allocate( SizeT sizeBytes, SizeT align )
    {
        return MemoryManager::FrameAlloc( sizeBytes, align );
    }

    void FrameAllocator::Deallocate( void * ptr )
    {
        // Do nothing
    }

    //===================================================================== FrameAllocator
}

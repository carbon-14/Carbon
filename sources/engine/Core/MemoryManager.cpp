#include "Core/MemoryManager.h"

#include "Core/NativeAllocator.h"
#include "Core/StackAllocator.h"

#include "Core/Assert.h"

namespace Core
{
    //====================================================================================
    // MemoryManager
    //====================================================================================

    static void *           frameAllocatorBuffer = 0;
    static StackAllocator   frameAllocator;

    void MemoryManager::Initialize( SizeT frameAllocatorSize )
    {
        CARBON_ASSERT( frameAllocatorBuffer == 0 );
        frameAllocatorBuffer = Malloc( frameAllocatorSize );
        frameAllocator.Initialize( frameAllocatorBuffer, frameAllocatorSize );
    }

    void MemoryManager::Destroy()
    {
        frameAllocator.Destroy();
        Free( frameAllocatorBuffer );
        frameAllocatorBuffer = 0;
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
}

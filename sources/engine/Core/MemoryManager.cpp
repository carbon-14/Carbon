#include "MemoryManager.h"

#include "DefaultAllocator.h"
#include "StackAllocator.h"

namespace Core
{
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
        return DefaultAllocator::Malloc( sizeBytes, align );
    }

    void * MemoryManager::Realloc( void * ptr, SizeT sizeBytes, SizeT align )
    {
        return DefaultAllocator::Realloc( ptr, sizeBytes, align );
    }

    void MemoryManager::Free( void * ptr )
    {
        DefaultAllocator::Free( ptr );
    }

    void MemoryManager::FrameUpdate()
    {
        frameAllocator.Clear();
    }

    void * MemoryManager::FrameAlloc( SizeT sizeBytes, SizeT align )
    {
        return frameAllocator.Allocate( sizeBytes, align );
    }
}

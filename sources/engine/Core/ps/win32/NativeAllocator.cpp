#include "Core/NativeAllocator.h"

#include <cstdlib>

void * NativeAllocator::Malloc( SizeT sizeBytes, SizeT align )
{
    return _aligned_malloc( sizeBytes, align );
}

void * NativeAllocator::Realloc( void * ptr, SizeT sizeBytes, SizeT align )
{
    return _aligned_realloc( ptr, sizeBytes, align );
}

void NativeAllocator::Free( void * ptr )
{
    return _aligned_free( ptr );
}

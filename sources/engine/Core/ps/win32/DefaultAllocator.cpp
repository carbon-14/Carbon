#include "DefaultAllocator.h"

#include <malloc.h>

namespace Core
{
    void * DefaultAllocator::Malloc( SizeT sizeBytes, SizeT align )
    {
        return _aligned_malloc( sizeBytes, align );
    }

    void * DefaultAllocator::Realloc( void * ptr, SizeT sizeBytes, SizeT align )
    {
        return _aligned_realloc( ptr, sizeBytes, align );
    }

    void DefaultAllocator::Free( void * ptr )
    {
        return _aligned_free( ptr );
    }
}

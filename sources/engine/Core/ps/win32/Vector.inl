#include "Core/Assert.h"

namespace Core
{
    template< SizeT X, SizeT Y, SizeT Z, SizeT W >
    Vector Swizzle( Vector v )
    {
        return _mm_shuffle_ps( v, v, _MM_SHUFFLE( W, Z, Y, X ) );
    }
}

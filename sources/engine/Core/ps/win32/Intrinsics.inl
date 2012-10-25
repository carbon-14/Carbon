#include "Core/Assert.h"

namespace Core
{
    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
    inline M128  Shuffle( M128 l, M128 r )
    {
        return _mm_shuffle_ps( l, r, _MM_SHUFFLE( W, Z, Y, X ) );
    }

    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
    inline M128 Swizzle( M128 v )
    {
        return Shuffle< X, Y, Z, W >( v, v );
    }

    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
    inline M128 Mask()
    {
        U32 mask[4] = { X * 0xFFFFFFFF, Y * 0xFFFFFFFF, Z * 0xFFFFFFFF, W * 0xFFFFFFFF };
        return _mm_load_ps( (float*)mask );
    }
}

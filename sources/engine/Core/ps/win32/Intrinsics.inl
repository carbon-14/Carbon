#include <xmmintrin.h>

#include "Core/Types.h"
#include "Core/Assert.h"

typedef __declspec(align(16)) float F128[4];        // storage usage
typedef __m128                      M128;           // compute usage


inline M128 Set( F32 x, F32 y, F32 z, F32 w )   { return _mm_set_ps( w, z, y, x );             }
inline M128 Splat( F32 v )                      { return _mm_set_ps1( v );                     }

inline M128 Load( F128 d )                      { return _mm_load_ps( d );                     }
inline void Store( F128 d, M128 v )             { _mm_store_ps( d, v );                        }

// Arithmetic
inline M128 Neg( M128 v )                       { return _mm_xor_ps( v, Splat( -0.0f ) );      }
inline M128 Add( M128 l, M128 r )               { return _mm_add_ps( l, r );                   }
inline M128 Sub( M128 l, M128 r )               { return _mm_sub_ps( l, r );                   }
inline M128 Mul( M128 l, M128 r )               { return _mm_mul_ps( l, r );                   }
inline M128 Div( M128 l, M128 r )               { return _mm_div_ps( l, r );                   }
inline M128 Sqrt( M128 v )                      { return _mm_sqrt_ps( v );                     }
inline M128 Rcp( M128 v )                       { return _mm_rcp_ps( v );                      }
inline M128 Rsqrt( M128 v )                     { return _mm_rsqrt_ps( v );                    }
inline M128 Min( M128 a, M128 b )               { return _mm_min_ps( a, b );                   }
inline M128 Max( M128 a, M128 b )               { return _mm_max_ps( a, b );                   }
inline M128 Abs( M128 v )                       { return _mm_andnot_ps( Splat( -0.0f ), v );   }
inline M128 Clamp( M128 v, M128 min, M128 max )
{
    v = Min( v, max );
    return Max( min, v );
}
inline M128 Lerp( M128 a, M128 b, M128 r )
{
    b = Sub( b, a );
    b = Mul( b, r );
    return Add( b, a );
}

inline M128  Floor( M128 v );                                                           // TODO
inline M128  Ceil( M128 v );                                                            // TODO
inline M128  Exp( M128 v );                                                             // TODO
inline M128  Log( M128 v );                                                             // TODO
inline M128  Sin( M128 angle );                                                         // TODO
inline M128  Cos( M128 angle );                                                         // TODO
inline M128  Tan( M128 angle );                                                         // TODO
inline M128  ASin( M128 v );                                                            // TODO
inline M128  ACos( M128 v );                                                            // TODO
inline M128  ATan( M128 v );                                                            // TODO
inline void  SinCos( M128 angle, M128& sin, M128& cos );                                // TODO

// Logical
inline M128  And( M128 l, M128 r )              { return _mm_and_ps( l, r );                   }
inline M128  AndNot( M128 l, M128 r )           { return _mm_andnot_ps( l, r );                }
inline M128  Or( M128 l, M128 r )               { return _mm_or_ps( l, r );                    }
inline M128  Xor( M128 l, M128 r )              { return _mm_xor_ps( l, r );                   }

// Comparison
inline M128 LessThan( M128 l, M128 r )          { return _mm_cmplt_ps( l, r );                 }
inline M128 LessEqual( M128 l, M128 r )         { return _mm_cmple_ps( l, r );                 }
inline M128 GreaterThan( M128 l, M128 r )       { return _mm_cmpgt_ps( l, r );                 }
inline M128 GreaterEqual( M128 l, M128 r )      { return _mm_cmpge_ps( l, r );                 }
inline M128 Equal( M128 l, M128 r )             { return _mm_cmpeq_ps( l, r );                 }
inline M128 NotEqual( M128 l, M128 r )          { return _mm_cmpneq_ps(l, r );                 }

// Misc
inline M128 UnpackXY( M128 l, M128 r )          { return _mm_unpacklo_ps( l, r );              }
inline M128 UnpackZW( M128 l, M128 r )          { return _mm_unpackhi_ps( l, r );              }
inline M128 MoveXY( M128 l, M128 r )            { return _mm_movelh_ps( l, r );                }
inline M128 MoveZW( M128 l, M128 r )            { return _mm_movehl_ps( l, r );                }

inline M128 Select( M128 l, M128 r, M128 mask )
{
    l = AndNot( mask, l );
    r = And( mask, r );
    return Or( l, r );
}

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
    __declspec(align(16)) U32 mask[4] = { X * 0xFFFFFFFF, Y * 0xFFFFFFFF, Z * 0xFFFFFFFF, W * 0xFFFFFFFF };
    return _mm_load_ps( (float*)mask );
}

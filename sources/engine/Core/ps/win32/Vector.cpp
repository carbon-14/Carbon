#include "Core/Vector.h"

#include "Core/Assert.h"

namespace Core
{
    inline Vector Vector4( F32 x, F32 y, F32 z, F32 w )     { return _mm_set_ps( w   , z   , y, x );        }
    inline Vector Vector3( F32 x, F32 y, F32 z )            { return _mm_set_ps( 0.0f, z   , y, x );        }
    inline Vector Vector2( F32 x, F32 y )                   { return _mm_set_ps( 0.0f, 0.0f, y, x );        }
    inline Vector Splat( F32 v )                            { return _mm_set_ps1( v );                      }

    inline void   Store( F128 d, Vector v )                 { _mm_store_ps( d, v );                         }
    inline Vector Load( F128 d )                            { return _mm_load_ps( d );                      }

    inline Vector Neg( Vector v )                           { return _mm_xor_ps( v, Splat( -0.0f ) );       }
    inline Vector Add( Vector l, Vector r )                 { return _mm_add_ps( l, r );                    }
    inline Vector Sub( Vector l, Vector r )                 { return _mm_sub_ps( l, r );                    }
    inline Vector Mul( Vector l, Vector r )                 { return _mm_mul_ps( l, r );                    }
    inline Vector Div( Vector l, Vector r )                 { return _mm_div_ps( l, r );                    }

    inline Vector operator-( Vector v )                     { return Neg( v );                              }
    inline Vector operator+( Vector l, Vector r )           { return Add( l, r );                           }
    inline Vector operator-( Vector l, Vector r )           { return Sub( l, r );                           }
    inline Vector operator*( Vector l, Vector r )           { return Mul( l, r );                           }
    inline Vector operator/( Vector l, Vector r )           { return Div( l, r );                           }

    inline Vector Abs( Vector v )                           { return _mm_andnot_ps( Splat( -0.0f ), v );    }
    inline Vector Min( Vector a, Vector b )                 { return _mm_min_ps( a, b );                    }
    inline Vector Max( Vector a, Vector b )                 { return _mm_max_ps( a, b );                    }

    inline Vector Clamp( Vector v, Vector min, Vector max )
    {
        v = Min( v, max );
        return Max( min, v );
    }

    inline Vector Lerp( Vector a, Vector b, Vector r )
    {
        b = Sub( b, a );
        b = Mul( b, r );
        return Add( b, a );
    }

    inline Vector Sqrt( Vector v )                          { return _mm_sqrt_ps( v );                      }

    inline Vector LessThan( Vector l, Vector r )            { return _mm_cmplt_ps( l, r );                  }
    inline Vector LessEqual( Vector l, Vector r )           { return _mm_cmple_ps( l, r );                  }
    inline Vector GreaterThan( Vector l, Vector r )         { return _mm_cmpgt_ps( l, r );                  }
    inline Vector GreaterEqual( Vector l, Vector r )        { return _mm_cmpge_ps( l, r );                  }
    inline Vector Equal( Vector l, Vector r )               { return _mm_cmpeq_ps( l, r );                  }
    inline Vector NotEqual( Vector l, Vector r )            { return _mm_cmpneq_ps(l, r );                  }

    inline Vector operator<( Vector l, Vector r )           { return LessThan( l, r );                      }
    inline Vector operator<=( Vector l, Vector r )          { return LessEqual( l, r );                     }
    inline Vector operator>( Vector l, Vector r )           { return GreaterThan( l, r );                   }
    inline Vector operator>=( Vector l, Vector r )          { return GreaterEqual( l, r );                  }
    inline Vector operator==( Vector l, Vector r )          { return Equal( l, r );                         }
    inline Vector operator!=( Vector l, Vector r )          { return NotEqual( l, r );                      }

    inline Vector Dot( Vector l, Vector r )
    {
        l = Mul( l, r );
        r = Swizzle< 1, 0, 3, 2 >( l );
        l = Add( l, r );
        r = Swizzle< 2, 3, 0, 1 >( l );
        return Add( l, r );
    }

    inline Vector Cross4( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 2, 3, 0 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 2, 3, 0 >( l );
        v = Mul( r, v );
        u = Sub( u, v );
        return Swizzle< 1, 2, 3, 0 >( u );
    }

    inline Vector Cross3( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 2, 3, 0 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 2, 3, 0 >( l );
        v = Mul( r, v );
        u = Sub( u, v );
        return Swizzle< 1, 2, 0, 3 >( u );
    }

    inline Vector Cross2( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 0, 2, 3 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 0, 2, 3 >( u );
        return Sub( v, u );
    }

    inline Vector SquareLength( Vector v )                  { return Dot( v, v );                           }
    inline Vector Length( Vector v )                        { return Sqrt( SquareLength( v ) );             }
    inline Vector Normalize( Vector v )                     { return Div( v, Length( v ) );                 }

    // Units
    inline Vector Zero4()                                   { return Splat( 0.0f );                         }
    inline Vector One4()                                    { return Splat( 1.0f );                         }
    inline Vector UnitX()                                   { return Vector4( 1.0f, 0.0f, 0.0f, 0.0f );     }
    inline Vector UnitY()                                   { return Vector4( 0.0f, 1.0f, 0.0f, 0.0f );     }
    inline Vector UnitZ()                                   { return Vector4( 0.0f, 0.0f, 1.0f, 0.0f );     }
    inline Vector UnitW()                                   { return Vector4( 0.0f, 0.0f, 0.0f, 1.0f );     }
}

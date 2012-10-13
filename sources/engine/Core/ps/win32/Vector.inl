#include "Core/Assert.h"

namespace Core
{
    Vector Vector4( F32 x, F32 y, F32 z, F32 w ){ return _mm_set_ps( w   , z   , y, x ); }
    Vector Vector3( F32 x, F32 y, F32 z )       { return _mm_set_ps( 0.0f, z   , y, x ); }
    Vector Vector2( F32 x, F32 y )              { return _mm_set_ps( 0.0f, 0.0f, y, x ); }
    Vector Splat( F32 v )                       { return _mm_set_ps1( v );               }

    F32 Get( Vector v, SizeT index )
    {
        CARBON_ASSERT( index < 4 );

        __declspec(align(16)) float data[4];
        _mm_store_ps( data, v );
        return data[ index ];
    }

    void Set( Vector& v, SizeT index, F32 value )
    {
        CARBON_ASSERT( index < 4 );

        __declspec(align(16)) float data[4];
        _mm_store_ps( data, v );
        data[ index ] = value;
        v = _mm_load_ps( data );
    }

    template< SizeT X, SizeT Y, SizeT Z, SizeT W >
    Vector Swizzle( Vector v )
    {
        return _mm_shuffle_ps( v, v, _MM_SHUFFLE( W, Z, Y, X ) );
    }

    Vector Neg( Vector v )            { return _mm_xor_ps( v, Splat( -0.0f ) ); }
    Vector Add( Vector l, Vector r )            { return _mm_add_ps( l, r );    }
    Vector Sub( Vector l, Vector r )            { return _mm_sub_ps( l, r );    }
    Vector Mul( Vector l, Vector r )            { return _mm_mul_ps( l, r );    }
    Vector Div( Vector l, Vector r )            { return _mm_div_ps( l, r );    }

    Vector operator-( Vector v )                { return Neg( v );              }
    Vector operator+( Vector l, Vector r )      { return Add( l, r );           }
    Vector operator-( Vector l, Vector r )      { return Sub( l, r );           }
    Vector operator*( Vector l, Vector r )      { return Mul( l, r );           }
    Vector operator/( Vector l, Vector r )      { return Div( l, r );           }

    Vector& operator+=( Vector& l, Vector r )   { l = Add( l, r ); return l;    }
    Vector& operator-=( Vector& l, Vector r )   { l = Sub( l, r ); return l;    }
    Vector& operator*=( Vector& l, Vector r )   { l = Mul( l, r ); return l;    }
    Vector& operator/=( Vector& l, Vector r )   { l = Div( l, r ); return l;    }

    Vector LessThan( Vector l, Vector r )       { return _mm_cmplt_ps( l, r );  }
    Vector LessEqual( Vector l, Vector r )      { return _mm_cmple_ps( l, r );  }
    Vector GreaterThan( Vector l, Vector r )    { return _mm_cmpgt_ps( l, r );  }
    Vector GreaterEqual( Vector l, Vector r )   { return _mm_cmpge_ps( l, r );  }
    Vector Equal( Vector l, Vector r )          { return _mm_cmpeq_ps( l, r );  }
    Vector NotEqual( Vector l, Vector r )       { return _mm_cmpneq_ps(l, r );  }

    Vector operator<( Vector l, Vector r )      { return LessThan( l, r );      }
    Vector operator<=( Vector l, Vector r )     { return LessEqual( l, r );     }
    Vector operator>( Vector l, Vector r )      { return GreaterThan( l, r );   }
    Vector operator>=( Vector l, Vector r )     { return GreaterEqual( l, r );  }
    Vector operator==( Vector l, Vector r )     { return Equal( l, r );         }
    Vector operator!=( Vector l, Vector r )     { return NotEqual( l, r );      }

    Vector Abs( Vector v )                          { return _mm_andnot_ps( Splat( -0.0f ), v );  }
    Vector Min( Vector l, Vector r )                { return _mm_min_ps( l, r );                  }
    Vector Max( Vector l, Vector r )                { return _mm_max_ps( l, r );                  }
    Vector Clamp( Vector v, Vector min, Vector max ){ return Max( min, Min( max, v ) );           }

    Vector Lerp( Vector l, Vector r, Vector ratio )
    {
        Vector u = Sub( l, r );
        Vector v = Mul( u, r );
        return Add( r, v );
    }

    Vector Sqrt( Vector v ) { return _mm_sqrt_ps( v ); }

    Vector Dot( Vector l, Vector r )
    {
        Vector u = Mul( l, r );
		Vector v = Swizzle< 1, 0, 3, 2 >( u );
		u = Add( u, v );
		v = Swizzle< 2, 3, 0, 1 >( u );
		return Add( u, v );
    }

    Vector Cross4( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 2, 3, 0 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 2, 3, 0 >( l );
        v = Mul( r, v );
        u = Sub( u, v );
        return Swizzle< 1, 2, 3, 0 >( u );
    }

    Vector Cross3( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 2, 3, 0 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 2, 3, 0 >( l );
        v = Mul( r, v );
        u = Sub( u, v );
        return Swizzle< 1, 2, 0, 3 >( u );
    }

    Vector Cross2( Vector l, Vector r )
    {
        Vector u = Swizzle< 1, 0, 2, 3 >( r );
        u = Mul( l, u );
        Vector v = Swizzle< 1, 0, 2, 3 >( u );
        return Sub( v, u );
    }

    Vector SquareLength( Vector v ) { return Dot( v, v );               }
    Vector Length( Vector v )       { return Sqrt( SquareLength( v ) ); }
    Vector Normalize( Vector v )    { return Div( v, Length( v ) );     }
}

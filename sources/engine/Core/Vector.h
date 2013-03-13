#pragma once
#ifndef _CORE_VECTOR_H
#define _CORE_VECTOR_H

#include "Core/TypeTraits.h"
#include "Core/Intrinsics.h"

/*
// [ x y z w ]
*/
typedef M128 Vector;

inline Vector Vector4( F32 x, F32 y, F32 z, F32 w = 1.0f )  { return Set( x, y, z   , w    );       }
inline Vector Vector3( F32 x, F32 y, F32 z )                { return Set( x, y, z   , 0.0f );       }
inline Vector Vector2( F32 x, F32 y )                       { return Set( x, y, 0.0f, 0.0f );       }

inline Bool IsVector( Vector v )
{
    F128 d;
    Store( d, v );
    return ( d[ 3 ] == 0.0f );
}

inline Bool IsVertex( Vector v )
{
    F128 d;
    Store( d, v );
    return ( d[ 3 ] == 1.0f );
}

inline Bool IsScalar( Vector v )
{
    F128 d;
    Store( d, v );
    return ( d[ 0 ] == d[ 1 ] )
        && ( d[ 1 ] == d[ 2 ] )
        && ( d[ 2 ] == d[ 3 ] );
}

inline Vector operator-( Vector v )                         { return Neg( v );                      }
inline Vector operator+( Vector l, Vector r )               { return Add( l, r );                   }
inline Vector operator-( Vector l, Vector r )               { return Sub( l, r );                   }
inline Vector operator*( Vector l, Vector r )               { return Mul( l, r );                   }
inline Vector operator/( Vector l, Vector r )               { return Div( l, r );                   }

// Logical
inline Vector operator<( Vector l, Vector r )               { return LessThan( l, r );              }
inline Vector operator<=( Vector l, Vector r )              { return LessEqual( l, r );             }
inline Vector operator>( Vector l, Vector r )               { return GreaterThan( l, r );           }
inline Vector operator>=( Vector l, Vector r )              { return GreaterEqual( l, r );          }
inline Vector operator==( Vector l, Vector r )              { return Equal( l, r );                 }
inline Vector operator!=( Vector l, Vector r )              { return NotEqual( l, r );              }

// Geometric
inline Vector Dot( Vector l, Vector r )
{
    l = Mul( l, r );
    r = Swizzle< 1, 0, 3, 2 >( l );
    l = Add( l, r );
    r = Swizzle< 2, 3, 0, 1 >( l );
    return Add( l, r );
}

inline Vector Cross( Vector l, Vector r )
{
    Vector u = Swizzle< 1, 2, 0, 3 >( r );
    u = Mul( l, u );
    Vector v = Swizzle< 1, 2, 0, 3 >( l );
    v = Mul( r, v );
    u = Sub( u, v );
    return Swizzle< 1, 2, 0, 3 >( u );
}

inline Vector SquareLength( Vector v )                      { return Dot( v, v );                   }
inline Vector Length( Vector v )                            { return Sqrt( SquareLength( v ) );     }
inline Vector Normalize( Vector v )                         { return Div( v, Length( v ) );         }

// Units
const Vector Zero4                                          = Splat( 0.0f );
const Vector One4                                           = Splat( 1.0f );
const Vector UnitX                                          = Set( 1.0f, 0.0f, 0.0f, 0.0f );
const Vector UnitY                                          = Set( 0.0f, 1.0f, 0.0f, 0.0f );
const Vector UnitZ                                          = Set( 0.0f, 0.0f, 1.0f, 0.0f );
const Vector UnitW                                          = Set( 0.0f, 0.0f, 0.0f, 1.0f );

#endif // _CORE_VECTOR_H

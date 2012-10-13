#pragma once
#ifndef _CORE_VECTOR_H
#define _CORE_VECTOR_H

#include "Core/DLL.h"
#include "Core/Types.h"
#include "Core/Intrinsics.h"

namespace Core
{
    /*
    // [ x y z w ]
    */
    typedef M128 Vector;

    inline Vector   Vector4( F32 x, F32 y, F32 z, F32 w = 1.0f );
    inline Vector   Vector3( F32 x, F32 y, F32 z );
    inline Vector   Vector2( F32 x, F32 y );
    inline Vector   Splat( F32 v );

    inline F32      Get( Vector v, SizeT index );
    inline void     Set( Vector& v, SizeT index, F32 value );

    template< SizeT X, SizeT Y, SizeT Z, SizeT W >
    inline Vector   Swizzle( Vector v );

    // Arithmetic
    inline Vector   Neg( Vector v );
    inline Vector   Add( Vector l, Vector r );
    inline Vector   Sub( Vector l, Vector r );
    inline Vector   Mul( Vector l, Vector r );
    inline Vector   Div( Vector l, Vector r );

    inline Vector   operator-( Vector v );
    inline Vector   operator+( Vector l, Vector r );
    inline Vector   operator-( Vector l, Vector r );
    inline Vector   operator*( Vector l, Vector r );
    inline Vector   operator/( Vector l, Vector r );

    inline Vector&  operator+=( Vector& l, Vector r );
    inline Vector&  operator-=( Vector& l, Vector r );
    inline Vector&  operator*=( Vector& l, Vector r );
    inline Vector&  operator/=( Vector& l, Vector r );

    inline Vector   Abs( Vector v );
    inline Vector   Min( Vector a, Vector b );
    inline Vector   Max( Vector a, Vector b );
    inline Vector   Clamp( Vector v, Vector min, Vector max );

    inline Vector   Floor( Vector v );                                      // TODO
    inline Vector   Ceil( Vector v );                                       // TODO

    inline Vector   Lerp( Vector left, Vector right, Vector ratio );

    inline Vector   Sqrt( Vector v );

    inline Vector   Exp( Vector v );                                        // TODO
    inline Vector   Log( Vector v );                                        // TODO

    inline Vector   Sin( Vector angle );                                    // TODO
    inline Vector   Cos( Vector angle );                                    // TODO
    inline Vector   Tan( Vector angle );                                    // TODO
    inline Vector   ASin( Vector v );                                       // TODO
    inline Vector   ACos( Vector v );                                       // TODO
    inline Vector   ATan( Vector v );                                       // TODO
    inline void     SinCos( Vector angle, Vector& sin, Vector& cos );       // TODO

    // Comparison
    inline Vector   LessThan( Vector l, Vector rt );
    inline Vector   LessEqual( Vector l, Vector r );
    inline Vector   GreaterThan( Vector l, Vector r );
    inline Vector   GreaterEqual( Vector l, Vector r );
    inline Vector   Equal( Vector l, Vector r );
    inline Vector   NotEqual( Vector l, Vector r );

    inline Vector   operator<( Vector l, Vector r );
    inline Vector   operator<=( Vector l, Vector r );
    inline Vector   operator>( Vector l, Vector r );
    inline Vector   operator>=( Vector l, Vector r );
    inline Vector   operator==( Vector l, Vector r );
    inline Vector   operator!=( Vector l, Vector r );

    // Geometry
    inline Vector  Dot( Vector l, Vector r );

    inline Vector  Cross4( Vector l, Vector r );
    inline Vector  Cross3( Vector l, Vector r );
    inline Vector  Cross2( Vector l, Vector r );

    inline Vector  SquareLength( Vector v );
    inline Vector  Length( Vector v );
    inline Vector  Normalize( Vector v );
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/Vector.inl"
#else
    #error Vector not implemented
#endif

#endif // _CORE_VECTOR_H

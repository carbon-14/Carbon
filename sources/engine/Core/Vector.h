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

    _CoreExport Vector  Vector4( F32 x, F32 y, F32 z, F32 w = 1.0f );
    _CoreExport Vector  Vector3( F32 x, F32 y, F32 z );
    _CoreExport Vector  Vector2( F32 x, F32 y );
    _CoreExport Vector  Splat4( F32 v );
    _CoreExport Vector  Splat3( F32 v );
    _CoreExport Vector  Splat2( F32 v );

    _CoreExport F32     At( Vector v, SizeT index );
    _CoreExport void    Set( Vector v, SizeT index, F32 value );

    template< SizeT X, SizeT Y, SizeT Z, SizeT W >
    Vector Swizzle< X, Y, Z, W >( Vector v );

    // Arithmetic
    _CoreExport Vector  Neg( Vector v );
    _CoreExport Vector  Add( Vector l, Vector r );
    _CoreExport Vector  Sub( Vector l, Vector r );
    _CoreExport Vector  Mul( Vector l, Vector r );
    _CoreExport Vector  Div( Vector l, Vector r );

    _CoreExport Vector  operator-( Vector v );

    _CoreExport Vector  operator+( Vector l, Vector r );
    _CoreExport Vector  operator+( F32 l, Vector r );
    _CoreExport Vector  operator+( Vector l, F32 r );

    _CoreExport Vector  operator-( Vector l, Vector r );
    _CoreExport Vector  operator-( F32 l, Vector r );
    _CoreExport Vector  operator-( Vector l, F32 r );

    _CoreExport Vector  operator*( Vector l, Vector r );
    _CoreExport Vector  operator*( F32 l, Vector r );
    _CoreExport Vector  operator*( Vector l, F32 r );

    _CoreExport Vector  operator/( Vector l, Vector r );
    _CoreExport Vector  operator/( F32 l, Vector r );
    _CoreExport Vector  operator/( Vector l, F32 r );

    _CoreExport Vector& operator+=( Vector& l, Vector r );
    _CoreExport Vector& operator+=( Vector& l, F32 r );
    _CoreExport Vector& operator-=( Vector& l, Vector r );
    _CoreExport Vector& operator-=( Vector& l, F32 r );
    _CoreExport Vector& operator*=( Vector& l, Vector r );
    _CoreExport Vector& operator*=( Vector& l, F32 r );
    _CoreExport Vector& operator/=( Vector& l, Vector r );
    _CoreExport Vector& operator/=( Vector& l, F32 r );

    // Geometry
    _CoreExport Vector  Dot4( Vector l, Vector r );
    _CoreExport Vector  Dot3( Vector l, Vector r );
    _CoreExport Vector  Dot2( Vector l, Vector r );

    _CoreExport Vector  Cross4( Vector l, Vector r );
    _CoreExport Vector  Cross3( Vector l, Vector r );
    _CoreExport Vector  Cross2( Vector l, Vector r );

    // Comparison
    _CoreExport Vector  LesserThan( Vector l, Vector rt );
    _CoreExport Vector  LesserEqual( Vector l, Vector r );
    _CoreExport Vector  GreaterThan( Vector l, Vector r );
    _CoreExport Vector  GreaterEqual( Vector l, Vector r );
    _CoreExport Vector  Equal( Vector l, Vector r );
    _CoreExport Vector  NotEqual( Vector l, Vector r );

    _CoreExport Vector  operator<( Vector l, Vector r );
    _CoreExport Vector  operator<( F32 l, Vector r );
    _CoreExport Vector  operator<( Vector l, F32 r );

    _CoreExport Vector  operator<=( Vector l, Vector r );
    _CoreExport Vector  operator<=( F32 l, Vector r );
    _CoreExport Vector  operator<=( Vector l, F32 r );

    _CoreExport Vector  operator>( Vector l, Vector r );
    _CoreExport Vector  operator>( F32 l, Vector r );
    _CoreExport Vector  operator>( Vector l, F32 r );

    _CoreExport Vector  operator>=( Vector l, Vector r );
    _CoreExport Vector  operator>=( F32 l, Vector r );
    _CoreExport Vector  operator>=( Vector l, F32 r );

    _CoreExport Vector  operator==( Vector l, Vector r );
    _CoreExport Vector  operator==( F32 l, Vector r );
    _CoreExport Vector  operator==( Vector l, F32 r );

    _CoreExport Vector  operator!=( Vector l, Vector r );
    _CoreExport Vector  operator!=( F32 l, Vector r );
    _CoreExport Vector  operator!=( Vector l, F32 r );
}

#endif // _CORE_VECTOR_H

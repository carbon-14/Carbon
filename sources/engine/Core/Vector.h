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
    _CoreExport Vector  Splat( F32 v );

    _CoreExport void    Store( F128 d, Vector v );
    _CoreExport Vector  Load( F128 d );

    template< SizeT X, SizeT Y, SizeT Z, SizeT W >
                Vector  Swizzle( Vector v );

    // Arithmetic
    _CoreExport Vector  Neg( Vector v );
    _CoreExport Vector  Add( Vector l, Vector r );
    _CoreExport Vector  Sub( Vector l, Vector r );
    _CoreExport Vector  Mul( Vector l, Vector r );
    _CoreExport Vector  Div( Vector l, Vector r );

    _CoreExport Vector  operator-( Vector v );
    _CoreExport Vector  operator+( Vector l, Vector r );
    _CoreExport Vector  operator-( Vector l, Vector r );
    _CoreExport Vector  operator*( Vector l, Vector r );
    _CoreExport Vector  operator/( Vector l, Vector r );

    _CoreExport Vector  Abs( Vector v );
    _CoreExport Vector  Min( Vector a, Vector b );
    _CoreExport Vector  Max( Vector a, Vector b );
    _CoreExport Vector  Clamp( Vector v, Vector min, Vector max );

    _CoreExport Vector  Floor( Vector v );                                      // TODO
    _CoreExport Vector  Ceil( Vector v );                                       // TODO

    _CoreExport Vector  Lerp( Vector a, Vector b, Vector r );

    _CoreExport Vector  Sqrt( Vector v );
    _CoreExport Vector  Exp( Vector v );                                        // TODO
    _CoreExport Vector  Log( Vector v );                                        // TODO

    _CoreExport Vector  Sin( Vector angle );                                    // TODO
    _CoreExport Vector  Cos( Vector angle );                                    // TODO
    _CoreExport Vector  Tan( Vector angle );                                    // TODO
    _CoreExport Vector  ASin( Vector v );                                       // TODO
    _CoreExport Vector  ACos( Vector v );                                       // TODO
    _CoreExport Vector  ATan( Vector v );                                       // TODO
    _CoreExport void    SinCos( Vector angle, Vector& sin, Vector& cos );       // TODO

    // Comparison
    _CoreExport Vector  LessThan( Vector l, Vector r );
    _CoreExport Vector  LessEqual( Vector l, Vector r );
    _CoreExport Vector  GreaterThan( Vector l, Vector r );
    _CoreExport Vector  GreaterEqual( Vector l, Vector r );
    _CoreExport Vector  Equal( Vector l, Vector r );
    _CoreExport Vector  NotEqual( Vector l, Vector r );

    _CoreExport Vector  operator<( Vector l, Vector r );
    _CoreExport Vector  operator<=( Vector l, Vector r );
    _CoreExport Vector  operator>( Vector l, Vector r );
    _CoreExport Vector  operator>=( Vector l, Vector r );
    _CoreExport Vector  operator==( Vector l, Vector r );
    _CoreExport Vector  operator!=( Vector l, Vector r );

    // Geometry
    _CoreExport Vector  Dot( Vector l, Vector r );

    _CoreExport Vector  Cross4( Vector l, Vector r );
    _CoreExport Vector  Cross3( Vector l, Vector r );
    _CoreExport Vector  Cross2( Vector l, Vector r );

    _CoreExport Vector  SquareLength( Vector v );
    _CoreExport Vector  Length( Vector v );
    _CoreExport Vector  Normalize( Vector v );

    // Units
    _CoreExport Vector  Zero4();
    _CoreExport Vector  One4();
    _CoreExport Vector  UnitX();
    _CoreExport Vector  UnitY();
    _CoreExport Vector  UnitZ();
    _CoreExport Vector  UnitW();
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/Vector.inl"
#endif

#endif // _CORE_VECTOR_H

#pragma once
#ifndef _CORE_INTRINSICS_H
#define _CORE_INTRINSICS_H

#include "Core/DLL.h"
#include "Core/Types.h"

#if defined( CARBON_PLATFORM_WIN32 )

#include <xmmintrin.h>

typedef __declspec(align(16)) float F128[4];    // storage usage
typedef __m128                      M128;       // compute usage

#else

#error Intrinsics not defined

#endif

namespace Core
{
    // Set/Store/Load
    _CoreExport M128  Set( F32 x, F32 y, F32 z, F32 w );
    _CoreExport M128  Splat( F32 v );

    _CoreExport M128  Load( F128 d );
    _CoreExport void  Store( F128 d, M128 v );

    // Arithmetic
    _CoreExport M128  Neg( M128 v );
    _CoreExport M128  Add( M128 l, M128 r );
    _CoreExport M128  Sub( M128 l, M128 r );
    _CoreExport M128  Mul( M128 l, M128 r );
    _CoreExport M128  Div( M128 l, M128 r );
    _CoreExport M128  Sqrt( M128 v );
    _CoreExport M128  Rcp( M128 v );
    _CoreExport M128  Rsqrt( M128 v );
    _CoreExport M128  Min( M128 a, M128 b );
    _CoreExport M128  Max( M128 a, M128 b );
    _CoreExport M128  Abs( M128 v );
    _CoreExport M128  Clamp( M128 v, M128 min, M128 max );
    _CoreExport M128  Lerp( M128 a, M128 b, M128 r );

    _CoreExport M128  Floor( M128 v );                                  // TODO
    _CoreExport M128  Ceil( M128 v );                                   // TODO
    _CoreExport M128  Exp( M128 v );                                    // TODO
    _CoreExport M128  Log( M128 v );                                    // TODO
    _CoreExport M128  Sin( M128 angle );                                // TODO
    _CoreExport M128  Cos( M128 angle );                                // TODO
    _CoreExport M128  Tan( M128 angle );                                // TODO
    _CoreExport M128  ASin( M128 v );                                   // TODO
    _CoreExport M128  ACos( M128 v );                                   // TODO
    _CoreExport M128  ATan( M128 v );                                   // TODO
    _CoreExport void  SinCos( M128 angle, M128& sin, M128& cos );       // TODO

    // Logical
    _CoreExport M128  And( M128 l, M128 r );
    _CoreExport M128  AndNot( M128 l, M128 r );
    _CoreExport M128  Or( M128 l, M128 r );
    _CoreExport M128  Xor( M128 l, M128 r );

    // Comparison
    _CoreExport M128  LessThan( M128 l, M128 r );
    _CoreExport M128  LessEqual( M128 l, M128 r );
    _CoreExport M128  GreaterThan( M128 l, M128 r );
    _CoreExport M128  GreaterEqual( M128 l, M128 r );
    _CoreExport M128  Equal( M128 l, M128 r );
    _CoreExport M128  NotEqual( M128 l, M128 r );

    // Misc
    _CoreExport M128  UnpackXY( M128 l, M128 r );
    _CoreExport M128  UnpackZW( M128 l, M128 r );
    _CoreExport M128  MoveXY( M128 l, M128 r );
    _CoreExport M128  MoveZW( M128 l, M128 r );

    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
                M128  Shuffle( M128 l, M128 r );

    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
                M128  Swizzle( M128 v );

    template< unsigned X, unsigned Y, unsigned Z, unsigned W >
                M128  Mask();

    _CoreExport M128  Select( M128 l, M128 r, M128 mask );
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/Intrinsics.inl"
#endif


#endif // _CORE_INTRINSICS_H

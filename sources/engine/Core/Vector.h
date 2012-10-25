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

    _CoreExport Bool    IsVector( Vector v );   // w = 0
    _CoreExport Bool    IsVertex( Vector v );   // w = 1
    _CoreExport Bool    IsScalar( Vector v );   // x = y = z = w

    // Arithmetic
    _CoreExport Vector  operator-( Vector v );
    _CoreExport Vector  operator+( Vector l, Vector r );
    _CoreExport Vector  operator-( Vector l, Vector r );
    _CoreExport Vector  operator*( Vector l, Vector r );
    _CoreExport Vector  operator/( Vector l, Vector r );

    // Comparison
    _CoreExport Vector  operator<( Vector l, Vector r );
    _CoreExport Vector  operator<=( Vector l, Vector r );
    _CoreExport Vector  operator>( Vector l, Vector r );
    _CoreExport Vector  operator>=( Vector l, Vector r );
    _CoreExport Vector  operator==( Vector l, Vector r );
    _CoreExport Vector  operator!=( Vector l, Vector r );

    // Geometic
    _CoreExport Vector  Dot( Vector l, Vector r );
    _CoreExport Vector  Cross( Vector l, Vector r );
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

#endif // _CORE_VECTOR_H

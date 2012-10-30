#pragma once
#ifndef _CORE_QUATERNION_H
#define _CORE_QUATERNION_H

#include "Core/Math.h"
#include "Core/Vector.h"

namespace Core
{
    inline Vector Quaternion( Vector axis, F32 angle )
    {
        F32 halfAngle = 0.5f * angle;

        F32 sin = Sin( halfAngle );
        F32 cos = Cos( halfAngle );

        Vector factor = Set( sin, sin, sin, cos );

        return Mul( factor, Select( axis, One4(), Mask< 0, 0, 0, 1 >() ) );
    }

    // Arithmetic
    inline Vector MulQuat( Vector l, Vector r )
    {
        Vector neg = Xor( r, Splat( -0.0f ) );

        Vector f0 = Shuffle< 1, 3, 0, 2 >( r, neg );
        Vector f1 = Shuffle< 2, 3, 0, 1 >( r, neg );
        Vector f2 = Shuffle< 0, 3, 1, 2 >( r, neg );

        // X
        Vector x = Swizzle< 0, 0, 0, 0 >( l );
        Vector p0 = Swizzle< 1, 3, 0, 2 >( f0 );
        x = Mul( x, p0 );

        // Y
        Vector y = Swizzle< 1, 1, 1, 1 >( l );
        y = Mul( y, f1 );

        // Z
        Vector z = Swizzle< 2, 2, 2, 2 >( l );
        Vector p2 = Swizzle< 2, 0, 1, 3 >( f2 );
        z = Mul( z, p2 );

        // W
        Vector w = Swizzle< 3, 3, 3, 3 >( l );
        w = Mul( w, r );

        Vector a0 = Add( x, y );
        Vector a1 = Add( z, w );

        return Add( a0, a1 );
    }

    inline Vector InverseQuat( Vector q )
    {
        const Vector sign = Set( -0.0f, -0.0f, -0.0f, 0.0f );
        return Xor( q, sign );
    }

    // Units
    inline Vector IdentityQuat()
    {
        return UnitW();
    }
}

#endif // _CORE_QUATERNION_H

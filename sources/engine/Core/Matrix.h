#pragma once
#ifndef _CORE_MATRIX_H
#define _CORE_MATRIX_H

#include "Core/Vector.h"

/*
// [ m00 m01 m02 m03 ]
// [ m10 m11 m12 m13 ]
// [ m20 m21 m22 m23 ]
// [ m30 m31 m32 m33 ]
*/
struct Matrix
{
    Vector m_column[ 4 ];
};

inline void Store( F128 * d, const Matrix& m )
{
    Store( d[ 0 ], m.m_column[ 0 ] );
    Store( d[ 1 ], m.m_column[ 1 ] );
    Store( d[ 2 ], m.m_column[ 2 ] );
    Store( d[ 3 ], m.m_column[ 3 ] );
}

inline Matrix Load( const F128 * d )
{
    Matrix m;
    m.m_column[ 0 ] = Load( d[ 0 ] );
    m.m_column[ 1 ] = Load( d[ 1 ] );
    m.m_column[ 2 ] = Load( d[ 2 ] );
    m.m_column[ 3 ] = Load( d[ 3 ] );
    return m;
}

// Arithmetic operators
inline Matrix Neg( const Matrix& m )
{
    Matrix r;
    r.m_column[ 0 ] = Neg( m.m_column[ 0 ] );
    r.m_column[ 1 ] = Neg( m.m_column[ 1 ] );
    r.m_column[ 2 ] = Neg( m.m_column[ 2 ] );
    r.m_column[ 3 ] = Neg( m.m_column[ 3 ] );
    return r;
}

inline Matrix Add( const Matrix& l, const Matrix& r )
{
    Matrix m;
    m.m_column[ 0 ] = Add( l.m_column[ 0 ], r.m_column[ 0 ] );
    m.m_column[ 1 ] = Add( l.m_column[ 1 ], r.m_column[ 1 ] );
    m.m_column[ 2 ] = Add( l.m_column[ 2 ], r.m_column[ 2 ] );
    m.m_column[ 3 ] = Add( l.m_column[ 3 ], r.m_column[ 3 ] );
    return m;
}

inline Matrix Sub( const Matrix& l, const Matrix& r )
{
    Matrix m;
    m.m_column[ 0 ] = Sub( l.m_column[ 0 ], r.m_column[ 0 ] );
    m.m_column[ 1 ] = Sub( l.m_column[ 1 ], r.m_column[ 1 ] );
    m.m_column[ 2 ] = Sub( l.m_column[ 2 ], r.m_column[ 2 ] );
    m.m_column[ 3 ] = Sub( l.m_column[ 3 ], r.m_column[ 3 ] );
    return m;
}

inline Vector Mul( const Matrix& m, const Vector& v )
{
    Vector x = Swizzle< 0, 0, 0, 0 >( v );
    Vector y = Swizzle< 1, 1, 1, 1 >( v );
    Vector z = Swizzle< 2, 2, 2, 2 >( v );
    Vector w = Swizzle< 3, 3, 3, 3 >( v );

    x = Mul( x, m.m_column[ 0 ] );
    y = Mul( y, m.m_column[ 1 ] );
    Vector l = Add( x, y );

    z = Mul( z, m.m_column[ 2 ] );
    w = Mul( w, m.m_column[ 3 ] );
    Vector r = Add( z, w );

    return Add( l, r );
}

inline Matrix Mul( const Matrix& l, const Matrix& r )
{
    Matrix m;
    m.m_column[ 0 ] = Mul( l, r.m_column[ 0 ] );
    m.m_column[ 1 ] = Mul( l, r.m_column[ 1 ] );
    m.m_column[ 2 ] = Mul( l, r.m_column[ 2 ] );
    m.m_column[ 3 ] = Mul( l, r.m_column[ 3 ] );
    return m;
}

inline Matrix Transpose( const Matrix& m )
{
    Vector tmp0	= UnpackXY( m.m_column[ 0 ], m.m_column[ 1 ] );
    Vector tmp1	= UnpackXY( m.m_column[ 2 ], m.m_column[ 3 ] );
    Vector tmp2	= UnpackZW( m.m_column[ 0 ], m.m_column[ 1 ] );
    Vector tmp3	= UnpackZW( m.m_column[ 2 ], m.m_column[ 3 ] );

    Matrix tr;
    tr.m_column[ 0 ] = MoveXY( tmp0, tmp1 );
    tr.m_column[ 1 ] = MoveZW( tmp1, tmp0 );
    tr.m_column[ 2 ] = MoveXY( tmp2, tmp3 );
    tr.m_column[ 3 ] = MoveZW( tmp3, tmp2 );
    return tr;
}

inline Matrix Inverse( const Matrix& m )
{
    Vector swap00   = UnpackXY( m.m_column[ 1 ], m.m_column[ 2 ] );
    Vector swap10   = UnpackXY( m.m_column[ 2 ], m.m_column[ 3 ] );
    Vector swap01   = UnpackZW( m.m_column[ 1 ], m.m_column[ 2 ] );
    Vector swap11   = UnpackZW( m.m_column[ 2 ], m.m_column[ 3 ] );

    Vector col0a    = Swizzle< 1, 1, 0, 0 >( swap00 );
    Vector col0b    = Swizzle< 1, 1, 1, 0 >( swap10 );

    Vector col1a    = Swizzle< 3, 3, 2, 2 >( swap00 );
    Vector col1b    = Swizzle< 3, 3, 3, 2 >( swap10 );

    Vector col2a	= Swizzle< 1, 1, 0, 0 >( swap01 );
    Vector col2b	= Swizzle< 1, 1, 1, 0 >( swap11 );

    Vector col3a	= Swizzle< 3, 3, 2, 2 >( swap01 );
    Vector col3b	= Swizzle< 3, 3, 3, 2 >( swap11 );

    Vector left, right;

    // 0 1
    left            = Mul( col0a, col1b );
    right           = Mul( col0b, col1a );
    Vector f01      = Sub( left, right );

    // 0 2
    left            = Mul( col0a, col2b );
    right           = Mul( col0b, col2a );
    Vector f02      = Sub( left, right );

    // 0 3
    left            = Mul( col0a, col3b );
    right           = Mul( col0b, col3a );
    Vector f03      = Sub( left, right );

    // 1 2
    left            = Mul( col1a, col2b );
    right           = Mul( col1b, col2a );
    Vector f12      = Sub( left, right );

    // 1 3
    left            = Mul( col1a, col3b );
    right           = Mul( col1b, col3a );
    Vector f13      = Sub( left, right );

    // 2 3
    left            = Mul( col2a, col3b );
    right           = Mul( col2b, col3a );
    Vector f23      = Sub( left, right );

    Vector tmplo    = UnpackXY( m.m_column[ 0 ], m.m_column[ 1 ] );
    Vector tmphi    = UnpackZW( m.m_column[ 0 ], m.m_column[ 1 ] );

    Vector f0       = Swizzle< 1, 0, 0, 0 >( tmplo );
    Vector f1       = Swizzle< 3, 2, 2, 2 >( tmplo );
    Vector f2       = Swizzle< 1, 0, 0, 0 >( tmphi );
    Vector f3       = Swizzle< 3, 2, 2, 2 >( tmphi );

    const Vector signA = Vector4( -0.0f, +0.0f, -0.0f, +0.0f );
    const Vector signB = Vector4( +0.0f, -0.0f, +0.0f, -0.0f );

    Vector a, b, c, dif, sum;

    a               = Mul( f1, f23 );
    b               = Mul( f2, f13 );
    c               = Mul( f3, f12 );
    dif             = Sub( a, b );
    sum             = Add( dif, c );
    Vector col0     = Xor( sum, signA );

    a               = Mul( f0, f23 );
    b               = Mul( f2, f03 );
    c               = Mul( f3, f02 );
    dif             = Sub( a, b );
    sum             = Add( dif, c );
    Vector col1     = Xor( sum, signB );

    a               = Mul( f0, f13 );
    b               = Mul( f1, f03 );
    c               = Mul( f3, f01 );
    dif             = Sub( a, b );
    sum             = Add( dif, c );
    Vector col2     = Xor( sum, signA );

    a               = Mul( f0, f12 );
    b               = Mul( f1, f02 );
    c               = Mul( f2, f01 );
    dif             = Sub( a, b );
    sum             = Add( dif, c );
    Vector col3     = Xor( sum, signB );

    Vector adjlo    = UnpackXY( col0, col1 );
    Vector adjhi    = UnpackXY( col2, col3 );
    Vector adj      = MoveXY( adjlo, adjhi );
    Vector det      = Dot( m.m_column[ 0 ], adj );
    Vector invDet   = Div( Splat( 1.0f ), det );

    Matrix inv;
    inv.m_column[ 0 ]   = Mul( col0, invDet );
    inv.m_column[ 1 ]   = Mul( col1, invDet );
    inv.m_column[ 2 ]   = Mul( col2, invDet );
    inv.m_column[ 3 ]   = Mul( col3, invDet );
    return inv;
}

// Transformation Matrix
inline Matrix TMatrix( Vector translation )
{
    Matrix m;
    m.m_column[ 0 ] = UnitX();
    m.m_column[ 1 ] = UnitY();
    m.m_column[ 2 ] = UnitZ();
    m.m_column[ 3 ] = Select( translation, One4(), Mask< 0, 0, 0, 1 >() );
    return m;
}

inline Matrix SMatrix( Vector scale )
{
    Matrix m;
    m.m_column[ 0 ] = Select( scale, Zero4(), Mask< 0, 1, 1, 1 >() );
    m.m_column[ 1 ] = Select( scale, Zero4(), Mask< 1, 0, 1, 1 >() );
    m.m_column[ 2 ] = Select( scale, Zero4(), Mask< 1, 1, 0, 1 >() );
    m.m_column[ 3 ] = UnitW();
    return m;
}

inline Matrix RMatrix( Vector rotation )
{
    const Vector signPNP    = Vector4( +2.0f, -2.0f, +2.0f, 0.0f );
    const Vector signNPP    = Vector4( -2.0f, +2.0f, +2.0f, 0.0f );
    const Vector signPPN    = Vector4( +2.0f, +2.0f, -2.0f, 0.0f );

    Vector yxw0 = Swizzle< 1, 0, 3, 2 >( rotation );
    yxw0        = Mul( signPNP, yxw0 );

    Vector zwx0 = Swizzle< 2, 3, 0, 1 >( rotation );
    zwx0		= Mul( signNPP, zwx0 );

    Vector wzy0 = Swizzle< 3, 2, 1, 0 >( rotation );
    wzy0        = Mul( signPPN, wzy0 );

    Vector xxxx = Swizzle< 0, 0, 0, 0 >( rotation );
    Vector yyyy = Swizzle< 1, 1, 1, 1 >( rotation );
    Vector zzzz = Swizzle< 2, 2, 2, 2 >( rotation );

    Matrix m;
    {
        Vector left     = Mul( zzzz, zwx0 );
        Vector right    = Mul( yyyy, yxw0 );
        Vector col0     = Sub( left, right );
        m.m_column[0]   = Add( col0, UnitX() );
    }
    {
        Vector left     = Mul( xxxx, yxw0 );
        Vector right    = Mul( zzzz, wzy0 );
        Vector col1     = Sub( left, right );
        m.m_column[1]   = Add( col1, UnitY() );
    }
    {
        Vector left     = Mul( yyyy, wzy0 );
        Vector right    = Mul( xxxx, zwx0 );
        Vector col2     = Sub( left, right );
        m.m_column[2]   = Add( col2, UnitZ() );
    }

    m.m_column[3] = UnitW();

    return m;
}

inline Matrix& Translate( Matrix& m, const Vector& translation )
{
    CARBON_ASSERT( IsVector( translation ) );

    m.m_column[ 3 ] = Add( m.m_column[ 3 ], translation );
    return m;
}

inline Matrix& Scale( Matrix& m, const Vector& scale )
{
    m.m_column[ 0 ] = Mul( m.m_column[ 0 ], Swizzle<0,0,0,0>( scale ) );
    m.m_column[ 1 ] = Mul( m.m_column[ 1 ], Swizzle<1,1,1,1>( scale ) );
    m.m_column[ 2 ] = Mul( m.m_column[ 2 ], Swizzle<2,2,2,2>( scale ) );
    return m;
}

inline Vector TransformVector( const Matrix& m, const Vector& v )
{
    Vector x = Swizzle< 0, 0, 0, 0 >( v );
    Vector y = Swizzle< 1, 1, 1, 1 >( v );
    Vector z = Swizzle< 2, 2, 2, 2 >( v );

    x = Mul( x, m.m_column[ 0 ] );
    y = Mul( y, m.m_column[ 1 ] );
    z = Mul( z, m.m_column[ 2 ] );

    Vector r = Add( x, y );
    r = Add( z, r );

    return r;
}

inline Vector TransformVertex( const Matrix& m, const Vector& v )
{
    Vector x = Swizzle< 0, 0, 0, 0 >( v );
    Vector y = Swizzle< 1, 1, 1, 1 >( v );
    Vector z = Swizzle< 2, 2, 2, 2 >( v );
    Vector w = m.m_column[ 3 ];

    x = Mul( x, m.m_column[ 0 ] );
    y = Mul( y, m.m_column[ 1 ] );
    Vector l = Add( x, y );

    z = Mul( z, m.m_column[ 2 ] );
    Vector r = Add( z, w );

    return Add( l, r );
}

// Units
inline Matrix Identity()
{
    Matrix m;
    m.m_column[ 0 ] = UnitX();
    m.m_column[ 1 ] = UnitY();
    m.m_column[ 2 ] = UnitZ();
    m.m_column[ 3 ] = UnitW();
    return m;
}

CARBON_DECLARE_POD_TYPE( Matrix );

#endif // _CORE_MATRIX_H

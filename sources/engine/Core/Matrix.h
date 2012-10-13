#pragma once
#ifndef _CORE_MATRIX_H
#define _CORE_MATRIX_H

#include "Core/DLL.h"

#include "Core/Vector.h"

namespace Core
{
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

    _CoreExport void    Store( F128 * d, const Matrix& m );
    _CoreExport Matrix  Load( F128 * d );

    // Arithmetic operators
    _CoreExport Matrix  Add( const Matrix& l, const Matrix& r );
    _CoreExport Matrix  Sub( const Matrix& l, const Matrix& r );
    _CoreExport Vector  Mul( const Matrix& m, const Vector& v );
    _CoreExport Vector  Mul( const Vector& v, const Matrix& m );
    _CoreExport Matrix  Mul( const Matrix& l, const Matrix& r );
    _CoreExport Matrix  Transpose( const Matrix& m );
    _CoreExport Matrix  Inverse( const Matrix& m );

    _CoreExport Matrix  operator+( const Matrix& l, const Matrix& r );
    _CoreExport Matrix  operator-( const Matrix& l, const Matrix& r );
    _CoreExport Vector  operator*( const Matrix& m, const Vector& v );
    _CoreExport Vector  operator*( const Vector& v, const Matrix& m );
    _CoreExport Matrix  operator*( const Matrix& l, const Matrix& r );

    // Transformation Matrix
    _CoreExport Matrix  PMatrix( Vector translation );
    _CoreExport Matrix  SMatrix( Vector scale );
    _CoreExport Matrix  RMatrix( Vector rotation );
    _CoreExport Matrix  TMatrix( Vector translation, Vector scale, Vector rotation );

    _CoreExport Matrix& Translate( Matrix& m, const Vector& translation );
    _CoreExport Matrix& Scale( Matrix& m, const Vector& scale );

    _CoreExport Vector  TransformVector( const Matrix& m, const Vector& vector );
    _CoreExport Vector  TransformVertex( const Matrix& m, const Vector& vertex );

    ////////////////////////////////////////////////////////////////
    // Units
    _CoreExport Matrix  Identity();
}

#endif // _CORE_MATRIX_H

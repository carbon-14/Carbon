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
        Vector col[ 4 ];
    };
}

#endif // _CORE_MATRIX_H

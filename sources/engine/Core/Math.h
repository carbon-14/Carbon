#pragma once
#ifndef _CORE_MATH_H
#define _CORE_MATH_H

#include "Core/DLL.h"
#include "Core/Types.h"

namespace Core
{
    class _CoreExport Math
    {
    public:
        F32 Pi();
        F32 TwoPi();
        F32 HalfPi();
        F32 PosInf();
        F32 NegInf();

        S32 IAbs( S32 value );
        S32 IMin( S32 a, S32 b );
        S32 IMax( S32 a, S32 b );
        S32 IClamp( S32 value, S32 min, S32 max );

        F32 Abs( F32 value );
        F32 Min( F32 a, F32 b );
        F32 Max( F32 a, F32 b );
        F32 Clamp( F32 value, F32 min, F32 max );
        F32 Floor( F32 value );
        F32 Ceil( F32 value );
        F32 Lerp( F32 left, F32 right, F32 ratio );

        F32 Sqrt( F32 value );
        F32 Exp( F32 value );
        F32 Log( F32 value );

        F32 Sin( F32 angle );
        F32 Cos( F32 angle );
        F32 Tan( F32 angle );
        F32 ASin( F32 value );
        F32 ACos( F32 value );
        F32 ATan( F32 value );
        void SinCos( F32 angle, F32& sin, F32& cos );
    };
}

#endif // _CORE_MATH_H

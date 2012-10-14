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
        static F32 Pi();
        static F32 TwoPi();
        static F32 HalfPi();

        static S32 IAbs( S32 value );
        static S32 IMin( S32 a, S32 b );
        static S32 IMax( S32 a, S32 b );
        static S32 IClamp( S32 value, S32 min, S32 max );

        static F32 Abs( F32 value );
        static F32 Min( F32 a, F32 b );
        static F32 Max( F32 a, F32 b );
        static F32 Clamp( F32 value, F32 min, F32 max );
        static F32 Floor( F32 value );
        static F32 Ceil( F32 value );
        static F32 Lerp( F32 left, F32 right, F32 ratio );

        static F32 Sqrt( F32 value );
        static F32 Exp( F32 value );
        static F32 Log( F32 value );

        static F32 Sin( F32 angle );
        static F32 Cos( F32 angle );
        static F32 Tan( F32 angle );
        static F32 ASin( F32 value );
        static F32 ACos( F32 value );
        static F32 ATan( F32 value );
    };
}

#endif // _CORE_MATH_H

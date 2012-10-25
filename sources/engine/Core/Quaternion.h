#pragma once
#ifndef _CORE_QUATERNION_H
#define _CORE_QUATERNION_H

#include "Core/Vector.h"

namespace Core
{
    _CoreExport Vector  Quaternion( Vector axis, F32 angle );

    // Arithmetic
    _CoreExport Vector  MulQuat( Vector l, Vector r );
    _CoreExport Vector  InverseQuat( Vector q );

    // Units
    _CoreExport Vector  IdentityQuat();
}

#endif // _CORE_QUATERNION_H

#pragma once
#ifndef _CORE_TIMEUTILS_H
#define _CORE_TIMEUTILS_H

#include "Types.h"
#include "DLL.h"

class _CoreExport TimeUtils
{
public:
    static U64 ClockTime();
    static F64 ClockFrequency();
    static F64 ClockPeriod();
};

#endif // _CORE_TIMEUTILS_H

#include "Core/TimeUtils.h"

#include <ctime>

namespace Core
{
    U64 TimeUtils::ClockTime()
    {
        return (U32)clock();
    }

    F64 TimeUtils::ClockFrequency()
    {
        return (F64)CLOCKS_PER_SEC;
    }

    F64 TimeUtils::ClockPeriod()
    {
        return (F64)( 1.0 / CLOCKS_PER_SEC );
    }
}

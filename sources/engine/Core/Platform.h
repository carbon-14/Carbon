#pragma once
#ifndef _CORE_PLATFORM_H
#define _CORE_PLATFORM_H

#include "Core/Types.h"
#include "Core/DLL.h"

namespace Core
{
    class _CoreExport Platform
    {
    public:
        static const Char * GetName();
    };
}

#endif // _CORE_PLATFORM_H

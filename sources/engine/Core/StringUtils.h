#pragma once
#ifndef _CORE_STRINGUTILS_H
#define _CORE_STRINGUTILS_H

#include "Core/Types.h"
#include "Core/DLL.h"

class _CoreExport StringUtils
{
public:
    static Char * FormatString( Char * dest, SizeT size, const Char * format, ... );

    static Char * StrCpy( Char * dest, SizeT size, const Char * src );
};

#endif // _CORE_STRINGUTILS_H

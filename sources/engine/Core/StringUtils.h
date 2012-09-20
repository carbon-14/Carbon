#pragma once
#ifndef _CORE_STRINGUTILS_H
#define _CORE_STRINGUTILS_H

#include "Types.h"
#include "DLL.h"

class _CoreExport StringUtils
{
public:
    static S32      FormatString( Char * dest, SizeT size, const Char * format, ... );

    static Char *   StrCpy( Char * dest, SizeT size, const Char * src );
};

#define CARBON_FORMAT_STRING( dest, size, format, ... ) StringUtils::FormatString( dest, size, format, __VA_ARGS__ )

#endif // _CORE_STRINGUTILS_H

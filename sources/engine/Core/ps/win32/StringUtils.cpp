#include "StringUtils.h"

#include <cstdarg>
#include <cstdio>
#include <string.h>

Char * StringUtils::FormatString( Char * dest, SizeT size, const Char * format, ... )
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf_s( dest, size, format, arglist );
    return dest;
}

Char * StringUtils::StrCpy( Char * dest, SizeT size, const Char * src )
{
    strcpy_s( dest, size, src );
    return dest;
}

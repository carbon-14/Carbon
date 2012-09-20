#include "StringUtils.h"

#include <cstdarg>
#include <cstdio>
#include <string.h>

S32 StringUtils::FormatString( Char * dest, SizeT size, const Char * format, ... )
{
    va_list arglist;
    va_start(arglist, format);
    return vsprintf_s( dest, size, format, arglist );
}

Char * StringUtils::StrCpy( Char * dest, SizeT size, const Char * src )
{
    strcpy_s( dest, size, src );
    return dest;
}
